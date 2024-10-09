import random as rand
from base64 import b64encode
content = """
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
void *my_malloc(size_t size);
void init_secmalloc(void);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);

void main(){
size_t i = 0;
"""
# rand.seed(4321232110)
id = []
assertions = {}
if __name__ == "__main__":
    for _ in range(10000):
        r = rand.randint(1,10)
        if(r>5):
            taille = rand.randint(1, 0x100)
            name = b64encode(rand.randbytes(0xa)).decode().replace("=", "").replace("/", "").replace("+", "")
            name = ''.join(filter(lambda x: not x.isdigit(), name))
            id.append(name)
            content+=f"unsigned char *{name} = my_malloc({taille});\n"
            o = rand.randint(0,0xff)
            content+=f"memset({name},{hex(o)}, {taille});\n"
            assertions[name] = [o, taille]
            
        if(r<3 and id != []):
            t = rand.choice(id)
            
            content+=f"for (i = 0; i < {assertions[t][1]}; i++)"+"{"+f"if({t}[i] != {hex(assertions[t][0])})"+"{"+"printf(\"Skill issue !\\n\");exit(1);"+"}"+"}\n"
            content+=f"my_free({t});\n"
            id.remove(t)
            assertions.pop(t)
        if(r<5 and r>3 and id != []):
            t = rand.choice(id)
            real = rand.randint(1, 0x100)
            if(real<assertions[t][1]):
                assertions[t][1] = real
            content+=f"{t} = my_realloc({t}, {real});\n"
            content+=f"for (i = 0; i < {assertions[t][1]}; i++)"+"{"+f"if({t}[i] != {hex(assertions[t][0])})"+"{"+"printf(\"Skill issue !\\n\");exit(1);"+"}"+"}\n"
            assertions.pop(t)
            id.remove(t)

# HEAP ERROR TEST
t = id[0]
content+=f"printf(\"Smash it !\\n\");\n{t}[{assertions[t][1]+1}] = ({t}[{assertions[t][1]+1}]|0xf)^0xe4;\nmy_free({t});\n"
content+="return;\n"
content+="fail:\n\tprintf(\"Skill issue !\\n\");exit(1);"
content+="\n}"
with open("src/random.c", "w") as f:
    print(content, file=f)
