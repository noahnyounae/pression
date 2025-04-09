./auto-delete.sh .
./time.sh make
./time.sh ./serialiser input.txt
./time.sh ./compressor input.bin
./time.sh ./decompressor input.prs
./size.sh input.txt && ./size.sh input.prs && ./size.sh input.prs.cfg
./auto-delete.sh .