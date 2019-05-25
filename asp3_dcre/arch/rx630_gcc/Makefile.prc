#
#		Makefileのプロセッサ依存部（M68040用）
#

#
#  GNU開発環境のターゲットアーキテクチャの定義
#
GCC_TARGET = rx-elf

#
#  プロセッサ依存部ディレクトリ名の定義
#
PRCDIR = $(SRCDIR)/arch/$(PRC)_$(TOOL)

#
#  ツール依存部ディレクトリ名の定義 
#
TOOLDIR = $(SRCDIR)/arch/$(TOOL)

#
#  コンパイルオプション
#
INCLUDES := $(INCLUDES) -I$(TOOLDIR) -I$(PRCDIR)
COPTS := $(COPTS) -ffunction-sections -fno-function-cse -fsigned-char -fdata-sections -mlittle-endian-data -mcpu=rx600 -m64bit-doubles
LDFLAGS := $(LDFLAGS) -flto

#
#  カーネルに関する定義
#
KERNEL_DIRS := $(KERNEL_DIRS) $(PRCDIR)
KERNEL_ASMOBJS := $(KERNEL_ASMOBJS) prc_support.o prc_test.o
KERNEL_COBJS := $(KERNEL_COBJS) prc_kernel_impl.o rx630_kernel_impl.o

#
#  コンフィギュレータ関係の変数の定義
#
CFG_TABS := $(CFG_TABS) --symval-table $(PRCDIR)/prc_sym.def
CFG_ASMOBJS := $(CFG_ASMOBJS) kernel_cfg_asm.o

#
#  オフセットファイル生成のための定義
#
TARGET_OFFSET_TRB = $(PRCDIR)/prc_offset.trb
