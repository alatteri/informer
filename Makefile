########################################################################
#                             Makefile                                 #
########################################################################
DIR_DEST = build
EXCLUDE = --exclude=.svn --exclude=\*.pyc --exclude=.DS_Store

UNAME = $(shell uname)
ifeq 'Darwin' '$(UNAME)'
	OSX = true
	PLATFORM = 'osx'
else
	LINUX = true
	PLATFORM = 'linux'
endif

DIR_CLIENT = $(DIR_DEST)/instinctual/informer
DIR_SERVER = $(DIR_DEST)/server

# --------------------
# try not to change the lines below
#
DIR_BIN = bin
DIR_LIB = lib
DIR_CONF = conf
DIR_LOGS = logs
DIR_MEDIA = media
DIR_SPARK = spark
DIR_UPLOADS = uploads
DIR_TEMPLATES = templates
DIR_THIRD_PARTY = third_party
DIR_THIRD_PARTY_SRC = third_party/src
DIR_THIRD_PARTY_PYTHON = third_party/python

THIRD_PARTY_JASPER=jasper-1.900.1
THIRD_PARTY_TIFF=tiff-3.8.2
THIRD_PARTY_IMAGE_MAGICK=ImageMagick-6.3.9
THIRD_PARTY_X264=x264
THIRD_PARTY_FFMPEG=ffmpeg
THIRD_PARTY_QT_FASTSTART=qt-faststart

MAKE = make
SHELL = /bin/sh
RSYNC = rsync -q -avz $(EXCLUDE)
UNTAR = tar zxvf

LIB_PYS = $(shell find $(DIR_LIB) -type f -name \*.py)
LIB_PYCS = $(LIB_PYS:.py=.pyc)
LIB_PYCS_SERVER_ONLY = %rest.pyc %responder.pyc %moviemaker.pyc %urls.pyc %views.pyc %mixins.pyc \
                       %models.pyc %custom_base.pyc %custom_xml.pyc %custom_json.pyc \
                       %rest_filelist.pyc %signals.pyc %manage.pyc

CLIENT_LIB_PYCS = $(filter-out $(LIB_PYCS_SERVER_ONLY), $(addprefix $(DIR_CLIENT)/, $(LIB_PYCS)))

SERVER_LIB_PYCS = $(addprefix $(DIR_SERVER)/, $(LIB_PYCS))
SERVER_THIRD_PARTY = $(abspath $(DIR_SERVER)/$(DIR_THIRD_PARTY))
SERVER_THIRD_PARTY_BIN = $(SERVER_THIRD_PARTY)/bin
SERVER_THIRD_PARTY_LIB = $(SERVER_THIRD_PARTY)/lib
SERVER_THIRD_PARTY_INCLUDE = $(SERVER_THIRD_PARTY)/include

all : client server

client : $(CLIENT_LIB_PYCS)
	@echo "Making client $(DIR_CLIENT)..."
	install -d "$(DIR_CLIENT)/$(DIR_BIN)"
	install -d -m 777 "$(DIR_CLIENT)/$(DIR_LOGS)"
	install -d "$(DIR_CLIENT)/$(DIR_SPARK)"
	install -d -m 777 "$(DIR_CLIENT)/$(DIR_UPLOADS)"
	install -d "$(DIR_CLIENT)/$(DIR_THIRD_PARTY)"

	install -m 755 "$(DIR_BIN)/informerd" "$(DIR_CLIENT)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/uploader"  "$(DIR_CLIENT)/$(DIR_BIN)"
	install -m 755 $(wildcard $(DIR_SPARK)/informer.spark*) "$(DIR_CLIENT)/.."

	$(RSYNC) "$(DIR_CONF)" "$(DIR_CLIENT)"
	$(RSYNC) $(DIR_THIRD_PARTY)/{bin,include,lib,python,share} "$(DIR_CLIENT)/$(DIR_THIRD_PARTY)" \
		--exclude=django --exclude=django_restapi --exclude=psycopg2\*

server : $(SERVER_LIB_PYCS) server_third_party
	@echo "Making server $(DIR_SERVER)..."
	install -d "$(DIR_SERVER)/$(DIR_BIN)"
	install -d "$(DIR_SERVER)/$(DIR_LOGS)"
	install -d "$(DIR_SERVER)/$(DIR_THIRD_PARTY)"

	install -m 755 "$(DIR_BIN)/fakedata"   "$(DIR_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/loopmaker"  "$(DIR_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/moviemaker" "$(DIR_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/runserver"  "$(DIR_SERVER)/$(DIR_BIN)"

	$(RSYNC) "$(DIR_CONF)"      "$(DIR_SERVER)"
	$(RSYNC) "$(DIR_MEDIA)"     "$(DIR_SERVER)" --exclude=2008
	$(RSYNC) "$(DIR_TEMPLATES)" "$(DIR_SERVER)"
	$(RSYNC) "$(DIR_THIRD_PARTY_PYTHON)" "$(DIR_SERVER)/$(DIR_THIRD_PARTY)"

	svn info | grep Revision | cut -d ' ' -f 2 > $(DIR_SERVER)/VERSION
	echo "INFORMER_VERSION = \"`cat $(DIR_SERVER)/VERSION`\";" > $(DIR_SERVER)/$(DIR_MEDIA)/js/version.js

$(LIB_PYCS) : $(LIB_PYS)
	python -c "import sys; \
		   sys.path.append('$(DIR_LIB)'); \
		   sys.path.append('$(DIR_THIRD_PARTY_PYTHON)'); \
		   import sitecustomize; \
		   import $(subst /,., $(subst $(DIR_LIB)/,,$(basename $@)))"
	
$(CLIENT_LIB_PYCS) : $(filter-out $(LIB_PYCS_SERVER_ONLY), $(LIB_PYCS))
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DIR_CLIENT)/,,$@)" "$@"

$(SERVER_LIB_PYCS) : $(LIB_PYCS)
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DIR_SERVER)/,,$@)" "$@"

server_third_party : server_image_magick server_ffmpeg

server_jasper :
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) jasper.tar.gz && \
	cd $(THIRD_PARTY_JASPER) && \
        ./configure --prefix=$(SERVER_THIRD_PARTY) \
	--disable-shared --with-pic --enable-static && \
	$(MAKE) && \
	$(MAKE) install)

server_tiff :
ifdef OSX
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) $(THIRD_PARTY_TIFF).tar.gz && \
	cd $(THIRD_PARTY_TIFF) && \
	./configure --prefix=$(SERVER_THIRD_PARTY) --disable-shared && \
	$(MAKE) && \
	$(MAKE) install)
endif

server_image_magick : server_jasper server_tiff
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) ImageMagick.tar.gz && \
	cd $(THIRD_PARTY_IMAGE_MAGICK) && \
	./configure  --prefix=$(SERVER_THIRD_PARTY) LDFLAGS=-L$(SERVER_THIRD_PARTY_LIB) \
	CFLAGS=-I$(SERVER_THIRD_PARTY_INCLUDE) --with-pic --enable-static \
	--without-freetype --without-perl --without-png --without-x \
	--with-xml --disable-shared && \
	$(MAKE) && \
	$(MAKE) install)

server_x264 :
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) $(THIRD_PARTY_X264).tar.gz && \
	cd $(THIRD_PARTY_X264) && \
	./configure --prefix=$(SERVER_THIRD_PARTY) --enable-pthread && \
	$(MAKE) && \
	$(MAKE) install)

server_ffmpeg : server_x264
ifdef OSX
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) $(THIRD_PARTY_FFMPEG).tar.gz && \
	cd $(THIRD_PARTY_FFMPEG) && \
	./configure --prefix=$(SERVER_THIRD_PARTY) --enable-gpl --enable-pp \
	--enable-swscaler --enable-pthreads --enable-libx264 \
	--extra-cflags=-I$(SERVER_THIRD_PARTY_INCLUDE) \
	--extra-ldflags=-L$(SERVER_THIRD_PARTY_LIB) \
	--disable-vhook --disable-mmx && \
	$(MAKE) && \
	$(MAKE) install)
else
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) $(THIRD_PARTY_FFMPEG).tar.gz && \
	cd $(THIRD_PARTY_FFMPEG) && \
	./configure --prefix=$(SERVER_THIRD_PARTY) --enable-gpl --enable-pp \
	--enable-swscaler --enable-pthreads --enable-libx264 \
	--extra-cflags=-I$(SERVER_THIRD_PARTY_INCLUDE) \
	--extra-ldflags=-L$(SERVER_THIRD_PARTY_LIB) && \
	$(MAKE) && \
	$(MAKE) install)
endif
	(cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_FFMPEG)/tools && \
	$(MAKE) $(THIRD_PARTY_QT_FASTSTART) && \
	cp $(THIRD_PARTY_QT_FASTSTART) $(SERVER_THIRD_PARTY_BIN))

dist :
	@echo "Creating client distribution..."
	(test -d $(DIR_CLIENT) && \
	cd $(DIR_DEST) && \
	tar cvfzp instinctual.tar.gz instinctual; \
	true)

	@echo "Creating server distribution..."
	(test -d $(DIR_SERVER) && \
	cd $(DIR_DEST) && \
	tar cvfzp server.tar.gz server; \
	true)

.PHONY : clean
clean :
	@echo "Making clean..."
	rm -Rf $(DIR_CLIENT) $(DIR_SERVER)
	rm -f $(LIB_PYCS)

	(test -d $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_JASPER) && \
	cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_JASPER) && $(MAKE) clean; true)

	(test -d $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_TIFF) && \
	cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_TIFF) && $(MAKE) clean; true)

	(test -d $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_IMAGE_MAGICK) && \
	cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_IMAGE_MAGICK) && $(MAKE) clean; true)

	(test -d $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_X264) && \
	cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_X264) && $(MAKE) clean; true)

	(test -d $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_FFMPEG) && \
	cd $(DIR_THIRD_PARTY_SRC)/$(THIRD_PARTY_FFMPEG) && $(MAKE) clean && \
	rm -f tools/$(THIRD_PARTY_QT_FASTSTART); true)
