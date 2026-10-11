"""Generic converter regression; emitted guest PE is inspected, never executed."""
from pathlib import Path
import hashlib,json,struct,subprocess,sys
sys.path.insert(0,str(Path(__file__).resolve().parent))
from test_windows_import_modules import executable

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def fixture():
 b=bytearray(0x4000);b[:7]=b'\x7fELF\x02\x01\x01'
 struct.pack_into('<HHI',b,16,3,62,1);struct.pack_into('<Q',b,32,64);struct.pack_into('<HHH',b,52,64,56,3)
 def ph(i,t,fl,off,va,size):struct.pack_into('<IIQQQQQQ',b,64+i*56,t,fl,off,va,va,size,size,8)
 ph(0,1,5,0x1000,0,0x1000);ph(1,1,6,0x2000,0x2000,0x1000)
 b[0x1020:0x1026]=b'\xb8\x07\0\0\0\xc3'
 strings=b'\0alias#A#A\0alias#B#A\0fixtureModule\0';b[0x2200:0x2200+len(strings)]=strings
 tags=[(5,0x2200),(10,len(strings)),(6,0x2400),(11,24),(4,0x2600),(7,0x2700),(8,48),(9,24),(0x61000043,21),(0,0)]
 ph(2,2,6,0x2000,0x2000,len(tags)*16)
 for i,(t,v) in enumerate(tags):struct.pack_into('<QQ',b,0x2000+i*16,t,v)
 struct.pack_into('<II',b,0x2600,1,3)
 for i,no in [(1,1),(2,11)]:struct.pack_into('<IBBHQQ',b,0x2400+i*24,no,0x12,3,1,0x20,6)
 for i in range(2):struct.pack_into('<QQq',b,0x2700+i*24,0x2800+i*8,((i+1)<<32)|1,0)
 return b
def main():
 stock,current,out=map(Path,sys.argv[1:]);assert not out.exists();out.mkdir()
 case=out/'input';(case/'sce_module').mkdir(parents=True)
 module=case/'sce_module/alias.prx';module.write_bytes(fixture());source=case/'input.elf';source.write_bytes(executable('alias.prx','alias#A#B','fixtureModule'))
 pins={str(p):sha(p) for p in (module,source)};rows={}
 for label,converter in [('stock',stock),('corrected',current)]:
  target=out/label;target.mkdir();output=target/'app.exe'
  args=[str(converter),'--windows','--sce-module-path',str(case),str(source),str(output)]
  r=subprocess.run(args,cwd=target,stdin=subprocess.DEVNULL,stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=30)
  rows[label]=dict(exitCode=r.returncode,stdout=r.stdout.decode(errors='replace'),stderr=r.stderr.decode(errors='replace'),converterSha256=sha(converter),outputExists=output.exists())
  (out/(label+'-stdout.txt')).write_bytes(r.stdout);(out/(label+'-stderr.txt')).write_bytes(r.stderr)
  if label=='stock':assert r.returncode==2 and 'Duplicate or empty export after stripping #: alias' in rows[label]['stderr'] and not output.exists()
  else:assert r.returncode==0 and output.is_file(),rows[label]
 guest=out/'corrected/app0/sce_module/alias.prx.guest.prx';pe=guest.read_bytes();nt=struct.unpack_from('<I',pe,0x3c)[0];opt=nt+24;sections=opt+struct.unpack_from('<H',pe,nt+20)[0]
 def offset(rva):
  for i in range(struct.unpack_from('<H',pe,nt+6)[0]):
   _,va,sz,raw=struct.unpack_from('<IIII',pe,sections+i*40+8)
   if va<=rva<va+sz:return raw+rva-va
  raise AssertionError('unmapped PE RVA')
 ed=offset(struct.unpack_from('<I',pe,opt+112)[0]);functions,names=struct.unpack_from('<II',pe,ed+20);assert functions==names==1
 fn,ns,ords=struct.unpack_from('<III',pe,ed+28);rva=struct.unpack_from('<I',pe,offset(fn))[0];no=offset(struct.unpack_from('<I',pe,offset(ns))[0]);name=pe[no:pe.index(b'\0',no)].decode();assert name=='alias'
 assert rva==0x10020 and all(struct.unpack_from('<Q',pe,offset(0x12800+i*8))[0]==0x140000000+rva for i in range(2))
 assert all(sha(Path(p))==h for p,h in pins.items())
 result=dict(stock=rows['stock'],corrected=rows['corrected'],fixtureHashes=pins,fixtureUnchanged=True,exportNames=[name],exportCount=functions,exportRva=rva,guestOutputSha256=sha(guest),bothRelocationTargetsVerified=True,noGuestExecution=True)
 (out/'results.json').write_text(json.dumps(result,indent=2)+'\n');print('GENERIC_STOCK_FAIL_CORRECTED_PASS_SAME_FIXTURE')
if __name__=='__main__':main()
