/*
 * xil_sprintf.c
 *
 *  Created on: Mar 18, 2021
 *      Author: streit
 */

#include "xil_types.h"
#include "ctype.h"
#include "stdarg.h"

typedef struct params_s {
    s32 len;
    s32 num1;
    s32 num2;
    char8 pad_character;
    s32 do_padding;
    s32 left_flag;
    s32 unsigned_flag;
} params_t;

void xil_sprintf(char* s, const char *ctrl1, ...);

static int spadding(char* s, const s32 l_flag, const struct params_s *par)
{
    s32 i;
    s32 padLen = 0;

    if ((par->do_padding != 0) && (l_flag != 0) && (par->len < par->num1)) {
		i=(par->len);
        for (; i<(par->num1); i++) {
            s[padLen++] = par->pad_character;
		}
    }
    return padLen;
}

static int souts(char* s, const char * lp, struct params_s *par)
{
    int strLen = 0;
    const char * LocalPtr;
	LocalPtr = lp;
    /* pad on left if needed                         */
	if(LocalPtr != NULL) {
		par->len = (s32)strlen( LocalPtr);
	}
    strLen += spadding(s+strLen, !(par->left_flag), par);

    /* Move string to the buffer                     */
    while (((*LocalPtr) != (char8)0) && ((par->num2) != 0)) {
		(par->num2)--;
        s[strLen++] = *LocalPtr;
		LocalPtr += 1;
}

    /* Pad on right if needed                        */
    /* CR 439175 - elided next stmt. Seemed bogus.   */
    /* par->len = strlen( lp)                      */
    strLen += spadding(s+strLen, par->left_flag, par);

    return strLen;
}

static int soutnum(char* s, const s32 n, const s32 base, struct params_s *par)
{
    int numLen = 0;
    s32 negative;
	s32 i;
    char8 outbuf[32];
    const char8 digits[] = "0123456789ABCDEF";
    u32 num;
    for(i = 0; i<32; i++) {
	outbuf[i] = '0';
    }

    /* Check if number is negative                   */
    if ((par->unsigned_flag == 0) && (base == 10) && (n < 0L)) {
        negative = 1;
		num =(-(n));
    }
    else{
        num = n;
        negative = 0;
    }

    /* Build number (backwards) in outbuf            */
    i = 0;
    do {
		outbuf[i] = digits[(num % base)];
		i++;
		num /= base;
    } while (num > 0);

    if (negative != 0) {
		outbuf[i] = '-';
		i++;
	}

    outbuf[i] = 0;
    i--;

    /* Move the converted number to the buffer and   */
    /* add in the padding where needed.              */
    par->len = (s32)strlen(outbuf);
    numLen += spadding(s+numLen, !(par->left_flag), par);
    while (&outbuf[i] >= outbuf) {
        s[numLen++] = outbuf[i];
		i--;
}
    numLen += spadding(s+numLen, par->left_flag, par);
    return numLen;
}

static s32 getnum( char ** linep)
{
    s32 n;
    s32 ResultIsDigit = 0;
    char * cptr;
    n = 0;
    cptr = *linep;
	if(cptr != NULL){
		ResultIsDigit = isdigit(((s32)*cptr));
	}
    while (ResultIsDigit != 0) {
		if(cptr != NULL){
			n = ((n*10) + (((s32)*cptr) - (s32)'0'));
			cptr += 1;
			if(cptr != NULL){
				ResultIsDigit = isdigit(((s32)*cptr));
			}
		}
		ResultIsDigit = isdigit(((s32)*cptr));
	}
    *linep = ((char * )(cptr));
    return(n);
}

void xil_sprintf(char* s, const char *ctrl1, ...)
{
	s32 Check;
    int sCounter = 0;
#if defined (__aarch64__)
    s32 long_flag;
#endif
    s32 dot_flag;

    params_t par;

    char8 ch;
    va_list argp;
    char8 *ctrl = (char8 *)ctrl1;

    va_start( argp, ctrl1);

    while ((ctrl != NULL) && (*ctrl != (char8)0)) {

        /* move format string chars to buffer until a  */
        /* format control is found.                    */
        if (*ctrl != '%') {
            s[sCounter++] = *ctrl;
			ctrl += 1;
            continue;
        }

        /* initialize all the flags for this format.   */
        dot_flag = 0;
#if defined (__aarch64__)
		long_flag = 0;
#endif
        par.unsigned_flag = 0;
		par.left_flag = 0;
		par.do_padding = 0;
        par.pad_character = ' ';
        par.num2=32767;
		par.num1=0;
		par.len=0;

 try_next:
		if(ctrl != NULL) {
			ctrl += 1;
		}
		if(ctrl != NULL) {
			ch = *ctrl;
		}
		else {
			ch = *ctrl;
		}

        if (isdigit((s32)ch) != 0) {
            if (dot_flag != 0) {
                par.num2 = getnum(&ctrl);
			}
            else {
                if (ch == '0') {
                    par.pad_character = '0';
				}
				if(ctrl != NULL) {
			par.num1 = getnum(&ctrl);
				}
                par.do_padding = 1;
            }
            if(ctrl != NULL) {
			ctrl -= 1;
			}
            goto try_next;
        }

        switch (tolower((s32)ch)) {
            case '%':
                s[sCounter++] ='%';
                Check = 1;
                break;

            case '-':
                par.left_flag = 1;
                Check = 0;
                break;

            case '.':
                dot_flag = 1;
                Check = 0;
                break;

            case 'l':
            #if defined (__aarch64__)
                long_flag = 1;
            #endif
                Check = 0;
                break;

            case 'u':
                par.unsigned_flag = 1;
                /* fall through */
            case 'i':
            case 'd':
                #if defined (__aarch64__)
                if (long_flag != 0){
			        outnum1((s64)va_arg(argp, s64), 10L, &par);
                }
                else {
                    sCounter += soutnum(s+sCounter, va_arg(argp, s32), 10L, &par);
                }
                #else
                    sCounter += soutnum(s+sCounter, va_arg(argp, s32), 10L, &par);
                #endif
				Check = 1;
                break;
            case 'p':
                #if defined (__aarch64__)
                par.unsigned_flag = 1;
			    outnum1((s64)va_arg(argp, s64), 16L, &par);
			    Check = 1;
                break;
                #endif
            case 'X':
            case 'x':
                par.unsigned_flag = 1;
                #if defined (__aarch64__)
                if (long_flag != 0) {
				    outnum1((s64)va_arg(argp, s64), 16L, &par);
				}
				else {
				    sCounter += soutnum(s+sCounter, (s32)va_arg(argp, s32), 16L, &par);
                }
                #else
                sCounter += soutnum(s+sCounter, (s32)va_arg(argp, s32), 16L, &par);
                #endif
                Check = 1;
                break;

            case 's':
                sCounter += souts(s+sCounter, va_arg( argp, char *), &par);
                Check = 1;
                break;

            case 'c':
                s[sCounter++] = va_arg( argp, s32);
                Check = 1;
                break;

            case '\\':
                switch (*ctrl) {
                    case 'a':
                        s[sCounter++] = ((char8)0x07);
                        break;
                    case 'h':
                        s[sCounter++] = ((char8)0x08);
                        break;
                    case 'r':
                        s[sCounter++] = ((char8)0x0D);
                        break;
                    case 'n':
                        s[sCounter++] = ((char8)0x0D);
                        s[sCounter++] = ((char8)0x0A);
                        break;
                    default:
                        s[sCounter++] = *ctrl;
                        break;
                }
                ctrl += 1;
                Check = 0;
                break;

            default:
		Check = 1;
		break;
        }
        if(Check == 1) {
			if(ctrl != NULL) {
				ctrl += 1;
			}
                continue;
        }
        goto try_next;
    }
    va_end( argp);
}
