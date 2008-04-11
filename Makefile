########################################################################
#                             Makefile                                 #
########################################################################
DEST_BASE = build
EXCLUDE = --exclude=.svn --exclude=\*.pyc --exclude=.DS_Store

UNAME = $(shell uname)
ifeq 'Darwin' '$(UNAME)'
	OSX = true
	PLATFORM = 'osx'
else
	LINUX = true
	PLATFORM = 'linux'
endif

DEST_SPARK = $(DEST_BASE)/instinctual/informer
DEST_SERVER = $(DEST_BASE)/server

# --------------------
# try not to change the lines below
#
export TMPDIR:=$(shell echo "`pwd`")/$(DEST_BASE)/tmp

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

SPARK_LIB_PYCS = $(filter-out $(LIB_PYCS_SERVER_ONLY), $(addprefix $(DEST_SPARK)/, $(LIB_PYCS)))
SPARK_THIRD_PARTY = /usr/discreet/sparks/instinctual/informer/$(DEST_SPARK)/$(DIR_THIRD_PARTY)
SPARK_THIRD_PARTY_BIN = $(SPARK_THIRD_PARTY)/bin
SPARK_THIRD_PARTY_LIB = $(SPARK_THIRD_PARTY)/lib
SPARK_THIRD_PARTY_INCLUDE = $(SPARK_THIRD_PARTY)/include

SERVER_LIB_PYCS = $(addprefix $(DEST_SERVER)/, $(LIB_PYCS))
SERVER_THIRD_PARTY = $(abspath $(DEST_SERVER)/$(DIR_THIRD_PARTY))
SERVER_THIRD_PARTY_BIN = $(SERVER_THIRD_PARTY)/bin
SERVER_THIRD_PARTY_LIB = $(SERVER_THIRD_PARTY)/lib
SERVER_THIRD_PARTY_INCLUDE = $(SERVER_THIRD_PARTY)/include

EXTRA_THIRD_PARTY_BINS = animate compare composite conjure display imgcmp imginfo \
			import jasper Magick-config Magick++-config MagickCore-config \
			MagickWand-config mogrify montage stream tmrdemo Wand-config

all :
	@echo "usage:"
	@echo "    make server"
	@echo "    make spark"

spark : $(SPARK_LIB_PYCS) spark_third_party
	@echo "Making spark $(DEST_SPARK)..."
	install -d "$(DEST_SPARK)/$(DIR_BIN)"
	install -d -m 777 "$(DEST_SPARK)/$(DIR_LOGS)"
	install -d "$(DEST_SPARK)/$(DIR_SPARK)"
	install -d -m 777 "$(DEST_SPARK)/$(DIR_UPLOADS)"
	install -d "$(DEST_SPARK)/$(DIR_THIRD_PARTY)"

	install -m 755 "$(DIR_BIN)/informerd" "$(DEST_SPARK)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/uploader"  "$(DEST_SPARK)/$(DIR_BIN)"
	install -m 755 $(wildcard $(DIR_SPARK)/informer.spark*) "$(DEST_SPARK)/.."

	install -d "$(DEST_SPARK)/$(DIR_CONF)"
	install -m 644 "$(DIR_CONF)/instinctual.ini.clean" "$(DEST_SPARK)/$(DIR_CONF)/instinctual.ini"

	$(RSYNC) $(DIR_THIRD_PARTY)/python "$(DEST_SPARK)/$(DIR_THIRD_PARTY)" \
		--exclude=django --exclude=django_restapi --exclude=psycopg2\*

	rm -Rf $(SPARK_THIRD_PARTY)/share
	svn info | grep Revision | cut -d ' ' -f 2 > $(DEST_SPARK)/VERSION

server : $(SERVER_LIB_PYCS) server_third_party
	@echo "Making server $(DEST_SERVER)..."
	install -d "$(DEST_SERVER)/$(DIR_BIN)"
	install -d "$(DEST_SERVER)/$(DIR_LOGS)"
	install -d "$(DEST_SERVER)/$(DIR_THIRD_PARTY)"

	install -m 755 "$(DIR_BIN)/firstrun"   "$(DEST_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/loopmaker"  "$(DEST_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/moviemaker" "$(DEST_SERVER)/$(DIR_BIN)"
	install -m 755 "$(DIR_BIN)/runserver"  "$(DEST_SERVER)/$(DIR_BIN)"

	install -d "$(DEST_SERVER)/$(DIR_CONF)"
	install -m 644 "$(DIR_CONF)/instinctual.ini.clean" "$(DEST_SERVER)/$(DIR_CONF)/instinctual.ini"

	$(RSYNC) "$(DIR_MEDIA)"     "$(DEST_SERVER)" --exclude=2008
	$(RSYNC) "$(DIR_TEMPLATES)" "$(DEST_SERVER)"
	$(RSYNC) "$(DIR_THIRD_PARTY_PYTHON)" "$(DEST_SERVER)/$(DIR_THIRD_PARTY)"

	svn info | grep Revision | cut -d ' ' -f 2 > $(DEST_SERVER)/VERSION
	echo "INFORMER_VERSION = \"`cat $(DEST_SERVER)/VERSION`\";" > $(DEST_SERVER)/$(DIR_MEDIA)/js/version.js

$(LIB_PYCS) : $(LIB_PYS)
	python -c "import sys; \
		   sys.path.append('$(DIR_LIB)'); \
		   sys.path.append('$(DIR_THIRD_PARTY_PYTHON)'); \
		   import sitecustomize; \
		   import $(subst /,., $(subst $(DIR_LIB)/,,$(basename $@)))"
	
$(SPARK_LIB_PYCS) : $(filter-out $(LIB_PYCS_SERVER_ONLY), $(LIB_PYCS))
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DEST_SPARK)/,,$@)" "$@"

$(SERVER_LIB_PYCS) : $(LIB_PYCS)
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DEST_SERVER)/,,$@)" "$@"

spark_third_party : spark_image_magick

server_third_party : server_image_magick server_ffmpeg

spark_jasper :
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) jasper.tar.gz && \
	cd $(THIRD_PARTY_JASPER) && \
	./configure --prefix=$(SPARK_THIRD_PARTY) \
	--disable-shared --with-pic --enable-static && \
	$(MAKE) && \
	$(MAKE) install)

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

spark_image_magick : spark_jasper
	(cd $(DIR_THIRD_PARTY_SRC) && \
	$(UNTAR) ImageMagick.tar.gz && \
	cd $(THIRD_PARTY_IMAGE_MAGICK) && \
	./configure  --prefix=$(SPARK_THIRD_PARTY) LDFLAGS=-L$(SPARK_THIRD_PARTY_LIB) \
	CFLAGS=-I$(SPARK_THIRD_PARTY_INCLUDE) --with-pic --enable-static \
	--without-freetype --without-perl --without-png --without-x \
	--with-xml --disable-shared && \
	$(MAKE) && \
	$(MAKE) install)
	rm $(addprefix $(SPARK_THIRD_PARTY_BIN)/, $(EXTRA_THIRD_PARTY_BINS))

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
	rm $(addprefix $(SERVER_THIRD_PARTY_BIN)/, $(EXTRA_THIRD_PARTY_BINS))

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
	@echo "Creating spark distribution..."
	(test -d $(DEST_SPARK) && \
	cd $(DEST_BASE) && \
	tar cvfzp ../dist/spark.r`cat instinctual/informer/VERSION`.tar.gz instinctual; \
	true)

	@echo "Creating server distribution..."
	(test -d $(DEST_SERVER) && \
	cd $(DEST_BASE) && \
	tar cvfzp ../dist/server.r`cat server/VERSION`.tar.gz server; \
	true)

clean :
	@echo "Making clean..."
	rm -Rf $(DEST_SPARK) $(DEST_SERVER)
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

.PHONY : clean dist server spark
