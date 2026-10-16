import assert from "node:assert/strict";
import { register } from "node:module";
import { pathToFileURL } from "node:url";
import path from "node:path";

register("./cdn-loader.mjs", import.meta.url, { data: { url: process.env.ANYPS5_TEST_ZIP_URL, path: process.env.ANYPS5_TEST_ZIP_PATH } });
const { Archive, DirectoryWriter } = await import(pathToFileURL(path.join(process.argv[2], "archive.mjs")));
const { BlobWriter, Uint8ArrayReader, ZipWriter } = await import(process.env.ANYPS5_TEST_ZIP_URL);
const header = new Uint8Array([127, 69, 76, 70]);

async function zipped(entries) {
  const writer = new ZipWriter(new BlobWriter(), { useWebWorkers: false, useCompressionStream: false });
  for (const [name, directory] of entries) await writer.add(name, directory ? undefined : new Uint8ArrayReader(header), { directory });
  return writer.close();
}

const parents = ["outer/", "outer/inner/", "outer/inner/title/"];
for (const directories of [[], parents, [...parents].reverse()]) {
  const archive = new Archive(await zipped([
    ...directories.map(name => [name, true]), ["outer/inner/title/main.elf", false]
  ]));
  try {
    const summary = await archive.inspect(() => {});
    assert.equal(summary.root, "outer/inner/title/");
    assert.equal(summary.name, "main.elf");
    assert.equal(summary.files, 1);
    assert.deepEqual((await archive.conversionInput("linux", false, () => {})).executable, header);
  } finally { await archive.close(); }
}

const folder = new Archive([{ path: "outer/inner/title/main.elf", file: new Blob([header]) }]);
try { assert.equal((await folder.inspect(() => {})).root, "outer/inner/title/"); }
finally { await folder.close(); }

const siblings = new Archive(await zipped([["outer/", true], ["outer/main.elf", false], ["other/resource", false]]));
try { await assert.rejects(siblings.inspect(() => {}), /exactly one executable ELF/); }
finally { await siblings.close(); }

for (const cleanupRejects of [false, true]) {
  const failure = new Error("input read failed");
  let aborted = false;
  const output = new WritableStream({ abort(reason) {
    aborted = true;
    assert.equal(reason, failure);
    if (cleanupRejects) throw new Error("abort failed");
  } });
  const source = new ReadableStream({ start(controller) { controller.error(failure); } });
  const writer = new DirectoryWriter({ async file() { return output; } });
  await assert.rejects(writer.add("example.bin", source), error => error === failure);
  assert.equal(aborted, true);
  assert.equal(output.locked, false);
  assert.equal(source.locked, false);
}

{
  const failure = new Error("progress failed");
  let cancelled = false;
  let aborted = false;
  const source = new ReadableStream({
    start(controller) { controller.enqueue(new Uint8Array([1])); },
    cancel(reason) { cancelled = true; assert.equal(reason, failure); throw new Error("cancel failed"); }
  });
  const output = new WritableStream({ abort(reason) { aborted = true; assert.equal(reason, failure); } });
  const writer = new DirectoryWriter({ async file() { return output; } });
  await assert.rejects(writer.add("example.bin", source, { onprogress() { throw failure; } }), error => error === failure);
  assert.equal(cancelled, true);
  assert.equal(aborted, true);
  assert.equal(output.locked, false);
  assert.equal(source.locked, false);
}

{
  const failure = new Error("blob read failed");
  let aborted = false;
  const output = new WritableStream({ abort(reason) {
    aborted = true;
    assert.equal(reason, failure);
    throw new Error("abort failed");
  } });
  const source = { size: 1, async init() {}, async readUint8Array() { throw failure; } };
  const writer = new DirectoryWriter({ async file() { return output; } });
  await assert.rejects(writer.add("example.bin", source), error => error === failure);
  assert.equal(aborted, true);
  assert.equal(output.locked, false);
}
