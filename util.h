#ifndef UTIL_H
#define UTIL_H

#define DEBUG

class util {
    public:
        util(void);
        ~util(void);

        static void hexdump(const unsigned char *buf, const unsigned int len, const unsigned int dump_width = 16);
        static void hexdump(const          char *buf, const unsigned int len, const unsigned int dump_width = 16) { hexdump((const unsigned char *)buf,(const unsigned int)len,dump_width); }
        static void hexdump(const unsigned char *buf, const          int len, const unsigned int dump_width = 16) { hexdump((const unsigned char *)buf,(const unsigned int)len,dump_width); }
        static void hexdump(const          char *buf, const          int len, const unsigned int dump_width = 16) { hexdump((const unsigned char *)buf,(const unsigned int)len,dump_width); }
	static void ascdump(unsigned char *buf, unsigned int len);
	static void ascdump(char *buf, unsigned int len) {ascdump((unsigned char *)buf,len);}
        static void printf(const char *fmt, ...); // uses sprintf and buffer
        static void printfln(const char *fmt, ...);
        static void msg(const char c); // can be switched off with DEBUG
        static void msg(const char *fmt, ...); // can be switched off with DEBUG
	static void msgln(const char *fmt, ...); // can be switched off with DEBUG
	static void msgln() { msgln(""); } // can be switched off with DEBUG
        static void print(const char c); 
        static void print(const char *str);
        static void println(const char *str);
        static void println() {println("");}
        static void debug(const char *str, unsigned int uint); // prints uint as hex and dec
        static void debugln(const char *str, unsigned int uint);
        static void debug(const char c); // prints c as character/. and its hex value
        static void debugln(const char c);
	static void endlessLoop(); // goes into an endless loop
	static int strappend(char *str1, const char *str2, const int max);
    private:
        const static size_t maxstr=128;
	static void var_printf(const char *fmt, va_list ap);
	static void var_printf(const char *str);
};

#endif /* UTIL_H */
