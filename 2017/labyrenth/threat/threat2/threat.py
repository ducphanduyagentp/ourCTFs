from os import listdir
from os.path import isfile, join

def to_nibbles(data):
    out = []
    for byte in data:
        byte = ord(byte)
        upper = (0xf0 & byte) >> 4
        lower = byte & 0xf
        out += hex(upper)[2:]
        out += hex(lower)[2:]
    return ''.join(out)

filepath = 'labyrenth/'
fnames = [f for f in listdir(filepath) if isfile(join(filepath, f))] # get directory listing
f_data = []

st='53568b'
#st='f6410c407406837908007424ff4904780b8b118802ff010fb6c0eb0c0fbec05150e8'
#st='54240c8b442408d1e9d1dbd1ead1d80bc975f4f7f38bf0f76424148bc88b442410f7e603d1720e3b54240c7708720f3b44240876094e2b4424101b54241433db2b4424081b54240cf7daf7d883da008bca8bd38bd98bc88bc65ec21000'
#st='83c40c3bc7750f8b460c84c0790f83e0fd89460ceb07834e0c2083cbff5f8b46088366040089065e8bc35b'
#st='945e08b45188945e48b450c8945e88b451c8945ec8b45208945f08365f4008365f8008365fc008965f4896df864a1000000008945d88d45d864a300000000c745c'
#st='535657fc8945fc33c0505050ff75fcff7514ff7510ff750cff7508e8'
#st = '5153568b742414b9030000008bc633db99f7f93bd375048bc6eb1683fa0175058d4602eb0c83fa028d460174048b4424088d0c85' # guessed fixed point
#st = '4d5a90000300000004000000ffff0000b800000000000000400000000000000000000000000000000000000000000000000000000000000000000000'
#st = '00000e1fba0e00b409cd21b8014ccd21546869732070726f6772616d2063616e6e6f742062652072756e20696e20444f53206d6f64652e0d0d0a2400000000000000'
#st = 'ccccccccccccccc558bec5'
# st = '740684e474d4eb965e5f8d42ff5bc38d42fe5e5f5bc38d42fd5e5f5bc38d42fc5e5f5bc3'
#st = '578bf983fa047231f7d983e103740c2bd1880783c70183e90175f68bc8c1e00803c18bc8c1e01003c18bca83e203c1e9027406f3ab85d2740a880783c70183ea0175f68b4424085fc38b442404c3'
#st = '8bc7ba0300000083e904720c83e00303c8ff2485'
#st = '8b45085e5fc9c3908a46038847038b45085e5fc9c38d49008a46038847038a46028847028b45085e5fc9c3908a46038847038a46028847028a46018847018b45085e5fc9c3'
#st = '9023d18a06880783c601c1e90283c70183f9087288f3a5ff2495'
#st='23d18a0688078a46018847018a4602c1e90288470283c60383c70383f90872ccf3a5ff2495'
#st = 'cccc558bec57568b750c8b4d108b7d088bc18bd103c63bfe76083bf80f82'
#st='74cf84c97451f7c20300000075eb0bd8578bc3c1e310560bd88b0abffffefe7e8bc18bf733cb03f003f983f1ff83f0ff33cf33c683c20481e100010181751c250001018174d32500010101750881e60000008075c45e5f5b33c0c38b42fc3'
#st='d42ff5bc38da424000000008d64240033c08a442408538bd8c1e0088b542408f7c20300000074158a0a83c2013'
#st='8b4d088945f08b450c8945f48b451440c745ec'
#st='feffff83c41c8b45fc8b5d0c8b631c8b6b20ffe033c0405bc9c3'
#st='eb6a6a018b450cff70188b450cff70148b450cff700c6a00ff75108b450cff7010ff7508e8'
counter = len(st)
data_indices = []

for fname in fnames:
    f = open(filepath + fname)
    print '[+] Processing', filepath + fname, 
    data = to_nibbles(f.read())
    data_pos = data.find(st)

    if data_pos != -1: # exclude files not relevant to us
        print '--> starting point found @', data_pos
        data_indices.append(data_pos)
        f_data.append(data)
    else:
        print '--> no starting point'

wildcards = 0

while True: # now search
    
    nib = ''
    match = True
    
    for hexstring, index in zip(f_data, data_indices):
        if nib == '':
            nib = hexstring[index + counter]
        else:
            if nib != hexstring[index + counter]:
                wildcards += 1
                st += '?'
                match = False
                break

    if match:
        st += hexstring[index + counter]
    counter += 1
    if wildcards > 308: # discrepancy threshold
        break
        
        
print '[+] Found signature:'
y = '' # format for yar-file
for i in range(0, len(st), 2):
    y += st[i:i+2] + ' '
print y[:-2] 

