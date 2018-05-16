#! python
import os
import subprocess
from waflib import Configure

VERSION = '1.0.0'
APPNAME = 'TrapezoidWave'

top = '.'
out = 'build'


@Configure.conf
def gcc_modifier_arm_none_eabi(cnf):
    ldscript = os.path.abspath('./ldscript/stm32f103xe.ld')

    v = cnf.env
    v.cprogram_PATTERN = '%s.elf'
    v.CFLAGS_cprogram = ['-W', '-Wall', '-g', '-mcpu=cortex-m3', '-mthumb']
    # for assembly compile
    # v.ASFLAGS = ['-W', '-Wall', '-g', '-mcpu=cortex-m3', '-mthumb']
    # v.AS_TGT_F = ['-o']
    # for linking
    v.LDFLAGS_cprogram = ['-mthumb', '-mcpu=cortex-m3', '-nostartfiles', '-Wl,--start-group', '-lc', '-lm', '-Wl,--end-group', '-specs=nano.specs', '-specs=nosys.specs', '-static', '-Wl,-cref,-u,Reset_Handler', '-Wl,--gc-sections', '-Wl,--defsym=malloc_getpagesize_P=0x80']
    cnf.env.prepend_value('LDFLAGS', ['-T', ldscript, '-Wl,-Map={}.map'.format(APPNAME)])
    # for objcopy
    cnf.xcheck_host_prog('OBJCOPY', 'objcopy')


def options(opt):
    opt.add_option('--cdb', dest='generate_cdb', default=False, action='store_true',  help='whether to generate compile_commands.json')


def configure(cnf):
    cnf.env['CHOST'] = 'arm-none-eabi'
    cnf.load('c cross_gnu')

    if cnf.options.generate_cdb:
        cnf.load('clang_compilation_database')
        cnf.env.append_value('CFLAGS', ['-isystem', 'd:/GNU/gcc-arm-none-eabi/arm-none-eabi/include'])
        print("compile_commands.json will be generated.")


def build(bld):
    StdRoot = 'e:/Programming/STM32/stsw-stm32054'
    CoreSupport = StdRoot+'/CMSIS/CM3/CoreSupport'
    DeviceSupport = StdRoot+'/CMSIS/CM3/DeviceSupport/ST/STM32F10x'
    Periph = StdRoot+'/STM32F10x_StdPeriph_Driver'

    Periph_files = ['misc.c', 'stm32f10x_exti.c', 'stm32f10x_gpio.c', 'stm32f10x_rcc.c', 'stm32f10x_tim.c']
    source_files = bld.path.ant_glob(['src/*.c'])
    source_files += bld.root.find_node(DeviceSupport).ant_glob('system_stm32f10x.c')
    source_files += bld.root.find_node(Periph).ant_glob(['src/'+periphfile for periphfile in Periph_files])

    bld.program(source=source_files, target=APPNAME, includes=['inc', Periph+'/inc', CoreSupport, DeviceSupport], defines=['STM32F10X_HD', 'USE_STDPERIPH_DRIVER'])
    bld(rule='${OBJCOPY} -O ihex ${SRC} ${TGT}', source='{}.elf'.format(APPNAME), target='{}.hex'.format(APPNAME))
    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}', source='{}.elf'.format(APPNAME), target='{}.bin'.format(APPNAME))


def download(ctx):
    suffix = 'hex'
    subprocess.call(['openocd', '-f', 'openocd.cfg', '-c', 'init', '-c', 'halt', '-c', "flash write_image erase build/{}.{}".format(APPNAME, suffix), '-c', 'reset', '-c', 'shutdown'])
