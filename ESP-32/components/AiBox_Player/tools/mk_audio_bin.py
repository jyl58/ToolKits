import sys
import os
import struct

__version__ = "1.1"

if __name__=="__main__":

    ftype = {"mp3":0, "wav":1}
    file_list = os.listdir("./")
    file_list = [x for x in file_list if ((x.endswith(".wav")) or (x.endswith(".mp3")))]
    fnum = len(file_list)
    head_len = 8 + 64 * fnum
    
    print ('__version__: ', __version__)
    print ('file_list: ', file_list)
    print ('fnum: ', fnum)
    print ('header_len: ', head_len)
    print ('--------------------')
    print (' ')
  
    bin_file = ''
    bin_file = struct.pack("<HHI",0x2053,fnum,0x0)
    
    song_bin = ''

    cur_addr = head_len
    songIdx = 0
    for fname in file_list:
        f = open(fname,'rb')
        print ('fname:', fname)
        
        fileType = fname.split(".")[-1].lower()
        
        print ('song index: ', songIdx)
        print ('file type: ', fileType)
        
        songLen = os.path.getsize(fname)
        print ('file len: ', songLen)
        
        songAddr = cur_addr
        print ('songAddr: ', songAddr)
        RFU = [0]*12
        
        
        print ('songLen: ', songLen)
        bin_file += struct.pack("<BBBBII"  ,0x28            #file tag
                                           ,songIdx         #song index
                                           ,ftype[fileType]
                                           ,0x0             #songVer
                                           ,songAddr
                                           ,songLen          #song length
                                           )
        bin_file += struct.pack("<12I",*RFU)
        bin_file += struct.pack("<I",0x0)

        
        song_bin += f.read()
        songIdx += 1
        pad = '\xff'*((4-songLen%4)%4)
        song_bin += pad
        cur_addr += (songLen + ((4-songLen%4)%4))
        
        print ('bin len:',len(bin_file))
        print ('--------------------')
        print('')
        f.close()
        pass
    
    bin_file += song_bin

    f= open("audio-esp.bin", "wb")
    f.write(bin_file)
    f.close()
