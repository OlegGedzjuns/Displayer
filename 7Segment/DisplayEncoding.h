// DisplayEncoding.h

#ifndef _DISPLAYENCODING_h
#define _DISPLAYENCODING_h

void GetSymbol(char c, bool segment[8], bool dot = false)
{
	switch (c)
	{
		case '1': { bool src[] = { 0,1,1,0,0,0,0 }; memcpy(segment, src, 7); break; }
		case '2': { bool src[] = { 1,1,0,1,1,0,1 }; memcpy(segment, src, 7); break; }
		case '3': { bool src[] = { 1,1,1,1,0,0,1 }; memcpy(segment, src, 7); break; }
		case '4': { bool src[] = { 0,1,1,0,0,1,1 }; memcpy(segment, src, 7); break; }
		case '5': { bool src[] = { 1,0,1,1,0,1,1 }; memcpy(segment, src, 7); break; }
		case '6': { bool src[] = { 1,0,1,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case '7': { bool src[] = { 1,1,1,0,0,0,0 }; memcpy(segment, src, 7); break; }
		case '8': { bool src[] = { 1,1,1,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case '9': { bool src[] = { 1,1,1,1,0,1,1 }; memcpy(segment, src, 7); break; }
		case '0': { bool src[] = { 1,1,1,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'A': { bool src[] = { 1,1,1,0,1,1,1 }; memcpy(segment, src, 7); break; }
		case 'B': { bool src[] = { 0,0,1,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'C': { bool src[] = { 1,0,0,1,1,1,0 }; memcpy(segment, src, 7); break; }
		case 'D': { bool src[] = { 0,1,1,1,1,0,1 }; memcpy(segment, src, 7); break; }
		case 'E': { bool src[] = { 1,0,0,1,1,1,1 }; memcpy(segment, src, 7); break; }
		case 'F': { bool src[] = { 1,0,0,0,1,1,1 }; memcpy(segment, src, 7); break; }
		default:  { bool src[] = { 0,0,0,0,0,0,0 }; memcpy(segment, src, 7); break; }
	}
	if (dot)
		segment[7] = 1;
	else
		segment[7] = 0;
}

#endif

