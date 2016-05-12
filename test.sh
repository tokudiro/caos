pushd test

../caos.exe -SV TestBuf.caos
../caos.exe -L A.caos

rm result.txt

echo 
echo -- TEST START
diff -u -w -B TestBuf.expect.c TestBuf.c >> result.txt
if test `wc -c < result.txt` -eq 0 ; then
  echo 'TestBuf.c OK'
else
  echo 'TestBuf.c NG'
fi

diff -u -w -B TestBuf.expect.h TestBuf.h >> result.txt
if test `wc -c < result.txt` -eq 0 ; then
  echo 'TestBuf.h OK'
else
  echo 'TestBuf.h NG'
fi

diff -u -w -B ../TestBuf_def.h TestBuf_define.h >> result.txt
if test `wc -c < result.txt` -eq 0 ; then
  echo 'TestBuf_define.h OK'
else
  echo 'TestBuf_define.h NG'
fi

diff -u -w -B A.expect.c A.c >> result.txt
if test `wc -c < result.txt` -eq 0 ; then
  echo 'A.c OK'
else
  echo 'A.c NG'
fi

echo -- TEST END
popd
