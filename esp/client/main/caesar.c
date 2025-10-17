#include "caesar.h"
#include <ctype.h>

static char rot_alpha(char c, int shift){
    if(c>='a'&&c<='z'){int b='a'; return (char)((((c-b)+shift)%26)+b);}
    if(c>='A'&&c<='Z'){int b='A'; return (char)((((c-b)+shift)%26)+b);}
    return c;
}
static char rot_digit(char c, int shift){
    if(c>='0'&&c<='9'){int b='0'; return (char)((((c-b)+(shift%10))%10)+b);}
    return c;
}

size_t caesar_encrypt_bytes(const char* pt, uint8_t shift,
                            uint8_t* out, size_t cap){
    if (!out || cap == 0) {
	return 0;
}
size_t w = 0;
out[w++] = shift % 26;

    if(!pt) return w;
    for(; *pt; ++pt){
        char c=*pt;
        if(isalpha((unsigned char)c)) c=rot_alpha(c, shift%26);
        else if(isdigit((unsigned char)c)) c=rot_digit(c, shift);
        if(w<cap) out[w++]=(uint8_t)c; else break;
    }
    return w;
}

size_t caesar_decrypt_bytes(const uint8_t* in, size_t len,
                            char* out, size_t cap){
    if(!in||len==0||!out||cap==0) return 0;
    int inv=(26-(in[0]%26))%26, invd=(10-((in[0]%26)%10))%10; size_t w=0;
    for(size_t i=1;i<len;++i){
        char c=(char)in[i];
        if(isalpha((unsigned char)c)) c=rot_alpha(c, inv);
        else if(isdigit((unsigned char)c)) c=rot_digit(c, invd);
        if(w+1<cap) out[w++]=c; else break;
    }
    if(w<cap) out[w]='\0';
    return w;
}
