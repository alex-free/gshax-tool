# gshax tool GNUMakefile by Alex Free
CC=gcc
CFLAGS=-Wall -Werror
VER=2.0.1

gshax: clean
	$(CC) $(CFLAGS) gshax.c -o gshax

clean:
	rm -rf gshax.exe gshax

linux-x86:
	make gshax CFLAGS="-m32 -static -Wall -Werror -Ofast"

linux-x86_64:
	make gshax CFLAGS="-static -Wall -Werror -Ofast"

windows-x86:
	make gshax CC="i686-w64-mingw32-gcc"

windows-x86_64:
	make gshax CC="x86_64-w64-mingw32-gcc"

linux-release:
	rm -rf gshax-tool-$(VER)-$(PLATFORM) gshax-tool-$(VER)-$(PLATFORM).zip
	mkdir gshax-tool-$(VER)-$(PLATFORM)
	cp -rv gshax images readme.md license.txt gshax-tool-$(VER)-$(PLATFORM)
	chmod -R 777 gshax-tool-$(VER)-$(PLATFORM)
	zip -r gshax-tool-$(VER)-$(PLATFORM).zip gshax-tool-$(VER)-$(PLATFORM)
	rm -rf gshax-tool-$(VER)-$(PLATFORM)

windows-release:
	rm -rf gshax-tool-$(VER)-$(PLATFORM) gshax-tool-$(VER)-$(PLATFORM).zip
	mkdir gshax-tool-$(VER)-$(PLATFORM)
	cp -rv gshax.exe images readme.md license.txt gshax-tool-$(VER)-$(PLATFORM)
	chmod -R 777 gshax-tool-$(VER)-$(PLATFORM)
	zip -r gshax-tool-$(VER)-$(PLATFORM).zip gshax-tool-$(VER)-$(PLATFORM)
	rm -rf gshax-tool-$(VER)-$(PLATFORM)

linux-x86-release: linux-x86
	make linux-release PLATFORM=linux_x86_static

linux-x86_64-release: linux-x86_64
	make linux-release PLATFORM=linux_x86_64_static

windows-x86-release: windows-x86
	make windows-release PLATFORM=windows_x86

windows-x86_64-release: windows-x86_64
	make windows-release PLATFORM=windows_x86_64

clean-zip: clean
	rm -rf *.zip

all: linux-x86-release linux-x86_64-release windows-x86-release windows-x86_64-release
