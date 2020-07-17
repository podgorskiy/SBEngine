#pragma once
#include <stdint.h>


// Based on solution by Jan Bergström
// https://stackoverflow.com/questions/36897781/how-to-uppercase-lowercase-utf-8-characters-in-c
inline uint32_t to_lower(uint32_t cp)
{
    if (cp < 0x80)                        // one octet
    {
	    if ((cp >= 0x41) && (cp <= 0x5A)) // ASCII
	    {
		    return cp + 0x20;
	    }
		return cp;
    }
	else if (cp < 0x800)                // two octets
	{
		uint32_t extChar = ((cp >> 6u) & 0x3fu) | 0xc0u;
	    auto c = (cp & 0x3fu) | 0x80u;

        switch (extChar)
        {
	        case 0xc3: // Latin 1
		        if ((c >= 0x80) && (c <= 0x9e) && (c != 0x97))
			        c += 0x20; // US ASCII shift
		        break;
	        case 0xc4: // Latin Exteneded
		        if ((c >= 0x80) && (c <= 0xb7) && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        else if ((c >= 0xb9) && (c <= 0xbe) && (c % 2)) // Odd
			        c++; // Next char is lwr
		        else if (c == 0xbf)
		        {
			        extChar = 0xc5;
			        c = 0x80;
		        }
		        break;
	        case 0xc5: // Latin Exteneded
		        if ((c >= 0x80) && (c <= 0x88) && (c % 2)) // Odd
			        c++; // Next char is lwr
		        else if ((c >= 0x8a) && (c <= 0xb7) && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        else if ((c >= 0xb9) && (c <= 0xbe) && (c % 2)) // Odd
			        c++; // Next char is lwr
		        break;
	        case 0xc6: // Latin Exteneded
		        switch (c)
		        {
			        case 0x82:
			        case 0x84:
			        case 0x87:
			        case 0x8b:
			        case 0x91:
			        case 0x98:
			        case 0xa0:
			        case 0xa2:
			        case 0xa4:
			        case 0xa7:
			        case 0xac:
			        case 0xaf:
			        case 0xb3:
			        case 0xb5:
			        case 0xb8:
			        case 0xbc:
				        c++; // Next char is lwr
				        break;
			        default:
				        break;
		        }
		        break;
	        case 0xc7: // Latin Exteneded
		        if (c == 0x84)
		        {
			        c = 0x86;
		        }
		        else if (c == 0x85)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0x87)
		        {
			        c = 0x89;
		        }
		        else if (c == 0x88)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0x8a)
		        {
			        c = 0x8c;
		        }
		        else if (c == 0x8b)
		        {
			        c++; // Next char is lwr
		        }
		        else if ((c >= 0x8d)
		                 && (c <= 0x9c)
		                 && (c % 2))
		        { // Odd
			        c++; // Next char is lwr
		        }
		        else if ((c >= 0x9e)
		                 && (c <= 0xaf)
		                 && (!(c % 2)))
		        { // Even
			        c++; // Next char is lwr
		        }
		        else if (c == 0xb1)
		        {
			        c = 0xb3;
		        }
		        else if (c == 0xb2)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0xb4)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0xb8)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0xba)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0xbc)
		        {
			        c++; // Next char is lwr
		        }
		        else if (c == 0xbe)
		        {
			        c++;
		        } // Next char is lwr
		        break;
	        case 0xc8: // Latin Exteneded
		        if ((c >= 0x80)
		            && (c <= 0x9f)
		            && (!(c % 2)))
		        { // Even
			        c++; // Next char is lwr
		        }
		        else if ((c >= 0xa2)
		                 && (c <= 0xb3)
		                 && (!(c % 2)))
		        { // Even
			        c++; // Next char is lwr
		        }
		        else if (c == 0xbb)
		        {
			        c++;
		        } // Next char is lwr
		        break;
	        case 0xcd: // Greek & Coptic
		        switch (c)
		        {
			        case 0xb0:
			        case 0xb2:
			        case 0xb6:
				        c++; // Next char is lwr
				        break;
			        default:
				        if (c == 0xbf)
				        {
					        extChar = 0xcf;
					        c = 0xb3;
				        }
				        break;
		        }
		        break;
	        case 0xce: // Greek & Coptic
		        if (c == 0x86)
			        c = 0xac;
		        else if (c == 0x88)
			        c = 0xad;
		        else if (c == 0x89)
			        c = 0xae;
		        else if (c == 0x8a)
			        c = 0xaf;
		        else if (c == 0x8c)
		        {
			        extChar = 0xcf;
			        c = 0x8c;
		        }
		        else if (c == 0x8e)
		        {
			        extChar = 0xcf;
			        c = 0x8d;
		        }
		        else if (c == 0x8f)
		        {
			        extChar = 0xcf;
			        c = 0x8e;
		        }
		        else if ((c >= 0x91)
		                 && (c <= 0x9f))
			        c += 0x20; // US ASCII shift
		        else if ((c >= 0xa0)
		                 && (c <= 0xab)
		                 && (c != 0xa2))
		        {
			        extChar = 0xcf;
			        c -= 0x20;
		        }
		        break;
	        case 0xcf: // Greek & Coptic
		        if (c == 0x8f)
			        c = 0xb4;
		        else if (c == 0x91)
			        c++; // Next char is lwr
		        else if ((c >= 0x98)
		                 && (c <= 0xaf)
		                 && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        else if (c == 0xb4)
			        c = 0x91;
		        else if (c == 0xb7)
			        c++; // Next char is lwr
		        else if (c == 0xb9)
			        c = 0xb2;
		        else if (c == 0xbb)
			        c++; // Next char is lwr
		        else if (c == 0xbd)
		        {
			        extChar = 0xcd;
			        c = 0xbb;
		        }
		        else if (c == 0xbe)
		        {
			        extChar = 0xcd;
			        c = 0xbc;
		        }
		        else if (c == 0xbf)
		        {
			        extChar = 0xcd;
			        c = 0xbd;
		        }

		        break;
	        case 0xd0: // Cyrillic
		        if ((c >= 0x80)
		            && (c <= 0x8f))
		        {
			        extChar = 0xd1;
			        c += 0x10;
		        }
		        else if ((c >= 0x90)
		                 && (c <= 0x9f))
			        c += 0x20; // US ASCII shift
		        else if ((c >= 0xa0)
		                 && (c <= 0xaf))
		        {
			        extChar = 0xd1;
			        c -= 0x20;
		        }
		        break;
	        case 0xd1: // Cyrillic supplement
		        if ((c >= 0xa0)
		            && (c <= 0xbf)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xd2: // Cyrillic supplement
		        if (c == 0x80)
			        c++; // Next char is lwr
		        else if ((c >= 0x8a)
		                 && (c <= 0xbf)
		                 && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xd3: // Cyrillic supplement
		        if ((c >= 0x81)
		            && (c <= 0x8e)
		            && (c % 2)) // Odd
			        c++; // Next char is lwr
		        else if ((c >= 0x90)
		                 && (c <= 0xbf)
		                 && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xd4: // Cyrillic supplement & Armenian
		        if ((c >= 0x80)
		            && (c <= 0xaf)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        else if ((c >= 0xb1)
		                 && (c <= 0xbf))
		        {
			        extChar = 0xd5;
			        c -= 0x10;
		        }
		        break;
	        case 0xd5: // Armenian
		        if ((c >= 0x80)
		            && (c <= 0x96)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        default:
	        	return cp;
        }

        return ((extChar << 6u) & 0x7ffu) + (c & 0x3fu);
	}
	else if (cp < 0x10000)               // three octets
    {
		uint32_t extChar = ((cp >> 12u) | 0xe0u) * 0x100 | ((cp >> 6u) & 0x3fu) | 0x80u;
	    auto c = (cp & 0x3fu) | 0x80u;

        switch (extChar)
        {
	        case 0xe182: // Three byte code
		        if ((c >= 0xa0)
		            && (c <= 0xbf))
		        {
			        extChar = 0xe183;
			        c -= 0x10;
		        }
		        break;
	        case 0xe183: // Georgian
		        if ((c >= 0x80)
		            && ((c <= 0x85)
		                || (c == 0x87))
		            || (c == 0x8d))
		        {
			        c += 0x30;
		        }
		        break;
	        case 0xe1b8: // Latin extened
		        if ((c >= 0x80)
		            && (c <= 0xbf)
		            && (!(c % 2)))
		        { // Even
			        c++;
		        } // Next char is lwr
		        break;
	        case 0xe1b9: // Latin extened
		        if ((c >= 0x80)
		            && (c <= 0xbf)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xe1ba: // Latin extened
		        if ((c >= 0x80)
		            && (c <= 0x94)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        else if ((c >= 0x9e)
		                 && (c <= 0xbf)
		                 && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xe1bb: // Latin extened
		        if ((c >= 0x80)
		            && (c <= 0xbf)
		            && (!(c % 2))) // Even
			        c++; // Next char is lwr
		        break;
	        case 0xe1bc: // Greek extened
		        if ((c >= 0x88)
		            && (c <= 0x8f))
			        c -= 0x08;
		        else if ((c >= 0x98)
		                 && (c <= 0x9f))
			        c -= 0x08;
		        else if ((c >= 0xa8)
		                 && (c <= 0xaf))
			        c -= 0x08;
		        else if ((c >= 0xb8)
		                 && (c <= 0x8f))
			        c -= 0x08;
		        break;
	        case 0xe1bd: // Greek extened
		        if ((c >= 0x88)
		            && (c <= 0x8d))
			        c -= 0x08;
		        else if ((c >= 0x98)
		                 && (c <= 0x9f))
			        c -= 0x08;
		        else if ((c >= 0xa8)
		                 && (c <= 0xaf))
			        c -= 0x08;
		        else if ((c >= 0xb8)
		                 && (c <= 0x8f))
			        c -= 0x08;
		        break;
	        case 0xe1be: // Greek extened
		        if ((c >= 0x88)
		            && (c <= 0x8f))
			        c -= 0x08;
		        else if ((c >= 0x98)
		                 && (c <= 0x9f))
			        c -= 0x08;
		        else if ((c >= 0xa8)
		                 && (c <= 0xaf))
			        c -= 0x08;
		        else if ((c >= 0xb8)
		                 && (c <= 0xb9))
			        c -= 0x08;
		        break;
	        case 0xe1bf: // Greek extened
		        if ((c >= 0x88)
		            && (c <= 0x8c))
			        c -= 0x08;
		        else if ((c >= 0x98)
		                 && (c <= 0x9b))
			        c -= 0x08;
		        else if ((c >= 0xa8)
		                 && (c <= 0xac))
			        c -= 0x08;
		        break;
	        default:
	        	return cp;
        }

        return (((extChar & 0xff00u) << 4u) & 0xffffu) + (((extChar & 0xffu) << 6u) & 0xfffu) + (c & 0x3fu);
    }
	else               // four octets
    {
		uint32_t extChar = ((cp >> 18u) | 0xf0u) * 0x10000 | (((cp >> 12u) & 0x3fu) | 0x80u) * 0x100 | ((cp >> 6u) & 0x3fu) | 0x80u;
	    auto c = (cp & 0x3fu) | 0x80u;

        switch (extChar)
        {

			case 0xf09092: // Osage
				if ((c >= 0xb0)
				    && (c <= 0xbf))
				{
					extChar = 0xf09093;
					c -= 0x18;
				}
		        break;
			case 0xf09093: // Osage
				if ((c >= 0x80)
				    && (c <= 0x93))
				{
					c += 0x18;
				}
		        break;
	        default:
	        	return cp;
        }
        return (((extChar & 0xff0000u) << 2u) & 0x1fffffu)
            + (((extChar & 0xff00u) << 4u) & 0x3ffffu)
            + (((extChar & 0xffu) << 6u) & 0xfffu)
            + (c & 0x3fu);
    }
}
