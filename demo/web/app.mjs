const $ = id => document.getElementById(id);
const stages = ["Converting executable", "Converting PRX modules", "Scanning syscalls", "Scanning instructions", "Converting instructions", "Analysing imports", "Building output image", "Writing output"];
let worker;
let file;
let summary;
let working = false;
let sink;
let downloadUrl;
let logs = [];
let release;
const canStream = typeof window.showSaveFilePicker === "function" && window.isSecureContext;
const supported = window.isSecureContext && window.Worker && window.WebAssembly && window.ReadableStream && window.WritableStream && window.crypto?.subtle;
$("save").querySelector('[value="disk"]').disabled = !canStream;
$("save").value = canStream ? "disk" : "memory";

function bytes(value) {
  if (value < 1024) return `${value} B`;
  const unit = value >= 1024 ** 3 ? 3 : value >= 1024 ** 2 ? 2 : 1;
  return `${(value / 1024 ** unit).toFixed(1)} ${["B", "KiB", "MiB", "GiB"][unit]}`;
}

function update() {
  const target = document.querySelector('input[name="target"]:checked').value;
  const libraryBytes = release?.platforms[target]?.expandedBytes;
  const estimate = summary && libraryBytes !== undefined ? Math.ceil(summary.totalBytes * 1.02) + summary.selectedBytes * 4 + libraryBytes + summary.files * 1024 + 8 * 1024 * 1024 : null;
  const tooLarge = estimate !== null && estimate > 256 * 1024 * 1024;
  $("save").querySelector('[value="memory"]').disabled = Boolean(tooLarge);
  const memoryBlocked = $("save").value === "memory" && tooLarge;
  $("save-note").textContent = $("save").value === "disk" ? "Writes the ZIP to your chosen file without holding it in memory." : "Up to 256 MiB including expanded files and output overhead.";
  $("browser-note").hidden = supported && canStream && !memoryBlocked;
  $("browser-note").textContent = !supported ? "This browser or connection does not support the converter. Use a current browser over HTTPS, or use the native relinker." : memoryBlocked ? "This archive exceeds the memory-download limit. Choose Save directly to disk. If that option is unavailable, open this page in desktop Chrome or Edge." : "Direct disk saving is unavailable in this browser or context. Small apps can still be downloaded in memory. For larger apps, use desktop Chrome or Edge over HTTPS.";
  $("convert").disabled = !supported || working || !summary || !release || memoryBlocked || (summary.warnings.length > 0 && !$("acknowledge").checked);
  $("ready-note").textContent = summary ? (memoryBlocked ? "A direct disk save is required for this archive." : release ? `Libraries: ${release.tag} · ZIP compression level 9` : "Loading release information…") : "Choose a ZIP or folder to get started.";
}

function lock(value) {
  for (const id of ["choose", "choose-folder", "change", "target", "save", "intel"]) $(id).disabled = value || !supported || (id === "choose-folder" && !("webkitdirectory" in $("folder")));
}

async function fail(message) {
  worker?.terminate();
  worker = null;
  working = false;
  summary = null;
  if (sink) {
    const active = sink;
    sink = null;
    try { await active.abort(); } catch (error) { message += `\nCould not discard the output file: ${error.message}`; }
  }
  $("error").textContent = message;
  $("error").hidden = false;
  $("status").hidden = true;
  $("cancel").hidden = true;
  $("drop").hidden = false;
  $("file").value = "";
  lock(false);
  update();
}

async function onMessage({ data }) {
  try {
    if (data.type === "progress") {
      if (data.message) {
        logs.push(data.message.trim());
        logs = logs.slice(-150);
        $("log").textContent = logs.join("\n");
        return;
      }
      $("phase").textContent = data.phase ?? stages[data.stage];
      $("current-file").textContent = data.input ?? "";
      if (Number.isFinite(data.total) && data.total > 0) {
        const percent = Math.min(100, Math.floor(data.completed / data.total * 100));
        $("progress").value = percent;
        $("percent").textContent = `${percent}%`;
      } else {
        $("progress").removeAttribute("value");
        $("percent").textContent = "";
      }
    } else if (data.type === "inspected") {
      summary = data.summary;
      working = false;
      $("selection").hidden = false;
      $("drop").hidden = true;
      $("archive-name").textContent = file.name;
      $("archive-info").textContent = `${summary.files} files · ${bytes(summary.totalBytes)} expanded · ${summary.modules} modules · ${bytes(summary.selectedBytes)} conversion input` + (summary.root ? ` · Root: ${summary.root}` : "");
      $("warning-list").replaceChildren(...summary.warnings.map(message => { const item = document.createElement("li"); item.textContent = message; return item; }));
      $("warnings").hidden = !summary.warnings.length;
      $("status").hidden = true;
      $("cancel").hidden = true;
      lock(false);
      update();
    } else if (data.type === "write") {
      if (!sink) throw new Error("No output file is open");
      await sink.write(data.bytes);
      worker?.postMessage({ type: "written" });
    } else if (data.type === "complete") {
      if (sink) { await sink.close(); sink = null; }
      working = false;
      $("done").hidden = false;
      $("status").hidden = true;
      $("cancel").hidden = true;
      $("result-info").textContent = `${bytes(data.size)} ZIP · ${data.release} system libraries · ${data.outputName}`;
      if (data.blob) {
        downloadUrl = URL.createObjectURL(data.blob);
        $("download").href = downloadUrl;
        $("download").download = `${data.outputName}.zip`;
        $("download").hidden = false;
      } else $("result-info").textContent += " · Saved to disk";
      summary = null;
      worker.terminate();
      worker = null;
      lock(false);
      update();
    } else if (data.type === "error") await fail(data.message);
  } catch (error) { await fail(error.message); }
}

async function selectFile(selected, files) {
  if (working || !supported || !selected) return;
  if (!files && !selected.name.toLowerCase().endsWith(".zip")) { await fail("Choose a ZIP archive."); return; }
  worker?.terminate();
  if (downloadUrl) { URL.revokeObjectURL(downloadUrl); downloadUrl = null; }
  summary = null;
  file = selected;
  working = true;
  logs = [];
  $("log").textContent = "";
  for (const id of ["error", "done", "download", "warnings", "selection"]) $(id).hidden = true;
  $("acknowledge").checked = false;
  $("status").hidden = false;
  $("cancel").hidden = false;
  $("phase").textContent = "Inspecting files";
  $("current-file").textContent = file.name;
  $("percent").textContent = "";
  $("progress").removeAttribute("value");
  lock(true);
  update();
  worker = new Worker(new URL("./worker.mjs", import.meta.url), { type: "module" });
  worker.onmessage = onMessage;
  worker.onerror = event => fail(event.message || "The converter worker stopped. Try a smaller app or use the native relinker.");
  worker.postMessage(files ? { type: "inspect", files } : { type: "inspect", file });
}

$("choose").onclick = () => $("file").click();
$("change").onclick = () => { $("drop").hidden = false; };
$("choose-folder").onclick = () => { $("folder").value = ""; $("folder").click(); };
$("folder").onchange = () => {
  const files = [...$("folder").files].map(file => ({ path: file.webkitRelativePath, file }));
  if (files.length) selectFile({ name: files[0].path.split("/")[0] }, files);
};
$("file").onchange = () => selectFile($("file").files[0]);
for (const type of ["dragenter", "dragover"]) $("drop").addEventListener(type, event => { event.preventDefault(); if (!working) $("drop").classList.add("dragging"); });
$("drop").ondragleave = () => $("drop").classList.remove("dragging");
$("drop").ondrop = event => { event.preventDefault(); $("drop").classList.remove("dragging"); if (working) return; if (event.dataTransfer.files.length !== 1) { fail("Drop one ZIP archive at a time."); return; } selectFile(event.dataTransfer.files[0]); };
for (const id of ["save", "target", "acknowledge"]) $(id).addEventListener("change", update);
$("cancel").onclick = () => fail("Cancelled. No completed ZIP was saved. Choose the ZIP or folder again to restart.");
$("convert").onclick = async () => {
  if ($("convert").disabled) return;
  working = true;
  lock(true);
  update();
  try {
    if ($("save").value === "disk") {
      const handle = await window.showSaveFilePicker({ suggestedName: file.name.replace(/\.zip$/iu, "") + "-" + document.querySelector('input[name="target"]:checked').value + ".zip", types: [{ description: "ZIP archive", accept: { "application/zip": [".zip"] } }] });
      sink = await handle.createWritable();
    }
    $("status").hidden = false;
    $("cancel").hidden = false;
    $("error").hidden = true;
    worker.postMessage({ type: "convert", target: document.querySelector('input[name="target"]:checked').value, toIntel: $("intel").checked, mode: $("save").value });
  } catch (error) {
    if (error.name === "AbortError") { working = false; lock(false); update(); }
    else await fail(error.message);
  }
};
$("theme").onclick = () => {
  const dark = document.documentElement.dataset.theme ? document.documentElement.dataset.theme === "dark" : matchMedia("(prefers-color-scheme: dark)").matches;
  document.documentElement.dataset.theme = dark ? "light" : "dark";
};
update();
lock(false);
if (supported) fetch("./release.json", { cache: "no-cache" }).then(response => { if (!response.ok) throw new Error(`Cannot load release information (${response.status})`); return response.json(); }).then(value => {
  if (!value.tag || !["windows", "linux"].every(target => Number.isSafeInteger(value.platforms?.[target]?.expandedBytes))) throw new Error("Invalid release information");
  release = value;
  update();
}).catch(error => fail(error.message));

if (!("webkitdirectory" in $("folder"))) {
  $("choose-folder").disabled = true;
  $("choose-folder").title = "Folder selection is unavailable in this browser. Choose a ZIP instead.";
}
