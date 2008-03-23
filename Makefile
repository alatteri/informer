########################################################################
#                             Makefile                                 #
########################################################################

DIR_DEST = build
EXCLUDE = --exclude=.svn --exclude=\*.pyc --exclude=.DS_Store

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
DIR_THIRD_PARTY_LIB = third_party/python

SHELL = /bin/sh
RSYNC = rsync -q -avz $(EXCLUDE)

LIB_PYS = $(shell find $(DIR_LIB) -type f -name \*.py)
LIB_PYCS = $(LIB_PYS:.py=.pyc)
LIB_PYCS_SERVER_ONLY = %rest.pyc %responder.pyc %moviemaker.pyc %urls.pyc %views.pyc %mixins.pyc \
                       %models.pyc %custom_base.pyc %custom_xml.pyc %custom_json.pyc \
                       %rest_filelist.pyc %signals.pyc %manage.pyc

CLIENT_LIB_PYCS = $(filter-out $(LIB_PYCS_SERVER_ONLY), $(addprefix $(DIR_CLIENT)/, $(LIB_PYCS)))
SERVER_LIB_PYCS = $(addprefix $(DIR_SERVER)/, $(LIB_PYCS))

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

server : $(SERVER_LIB_PYCS)
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
	$(RSYNC) $(DIR_THIRD_PARTY)/{bin,include,lib,python,share} "$(DIR_SERVER)/$(DIR_THIRD_PARTY)"

	echo "INFORMER_VERSION = \"`svn info | grep Revision`\";" > $(DIR_SERVER)/$(DIR_MEDIA)/js/version.js

$(LIB_PYCS) : $(LIB_PYS)
	python -c "import sys; \
		   sys.path.append('$(DIR_LIB)'); \
		   sys.path.append('$(DIR_THIRD_PARTY_LIB)'); \
		   import sitecustomize; \
		   import $(subst /,., $(subst $(DIR_LIB)/,,$(basename $@)))"
	
$(CLIENT_LIB_PYCS) : $(filter-out $(LIB_PYCS_SERVER_ONLY), $(LIB_PYCS))
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DIR_CLIENT)/,,$@)" "$@"

$(SERVER_LIB_PYCS) : $(LIB_PYCS)
	install -d "$(dir $@)"
	install -m 644 "$(subst $(DIR_SERVER)/,,$@)" "$@"

.PHONY : clean
clean :
	@echo "Making clean..."
	rm -Rf $(DIR_DEST)
	rm -f $(LIB_PYCS)
