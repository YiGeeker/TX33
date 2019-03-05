#! python
from waflib import Configure, Logs, Errors, TaskGen
from waflib.Build import BuildContext

VERSION = '1.0.0'
APPNAME = 'TX33'

top = '.'
out = 'build'


@Configure.conf
def gcc_modifier_arm_none_eabi(cnf):
    ldscript = cnf.srcnode.find_node('ldscript/stm32f103xe.ld')

    v = cnf.env
    v.cprogram_PATTERN = '%s.elf'
    v.CFLAGS_cprogram = ['-W', '-Wall', '-g', '-mcpu=cortex-m3', '-mthumb']
    v.LDFLAGS_cprogram = ['-mthumb', '-mcpu=cortex-m3', '-nostartfiles', '-Wl,--start-group', '-lc', '-lm', '-Wl,--end-group', '-specs=nano.specs', '-specs=nosys.specs', '-static', '-Wl,-cref,-u,Reset_Handler', '-Wl,--gc-sections', '-Wl,--defsym=malloc_getpagesize_P=0x80']
    cnf.env.prepend_value('LDFLAGS', ['-T', ldscript.bldpath(), '-Wl,-Map={}.map'.format(APPNAME)])
    # for objcopy
    cnf.xcheck_host_prog('OBJCOPY', 'objcopy')


def options(opt):
    opt.add_option('--cdb', dest='generate_cdb', default=False, action='store_true',  help='whether to generate compile_commands.json')


def configure(cnf):
    cnf.env['CHOST'] = 'arm-none-eabi'
    cnf.load('c cross_gnu')
    cnf.env.PREFIX = cnf.srcnode.abspath()
    cnf.env.CLANG_COMPILATION_DATABASE = 0

    if cnf.options.generate_cdb:
        cnf.load('clang_compilation_database')
        cnf.env.CLANG_COMPILATION_DATABASE = 1
        Logs.info("compile_commands.json will be generated.")


def build(bld):
    StdRoot = bld.srcnode.find_dir('STM32F10x_StdPeriph_Lib')
    CoreSupport = StdRoot.find_dir('/CMSIS/CM3/CoreSupport')
    DeviceSupport = StdRoot.find_dir('/CMSIS/CM3/DeviceSupport/ST/STM32F10x')
    Periph = StdRoot.find_dir('/STM32F10x_StdPeriph_Driver')

    Periph_files = ['misc.c', 'stm32f10x_exti.c', 'stm32f10x_gpio.c', 'stm32f10x_rcc.c', 'stm32f10x_tim.c']
    source_files = bld.srcnode.ant_glob('src/*.c')
    source_files += [DeviceSupport.find_node('system_stm32f10x.c')]
    source_files += [Periph.find_node('src/{}'.format(periphfile)) for periphfile in Periph_files]

    bld.program(source=source_files, target=APPNAME, includes=['inc', Periph.find_dir('inc').srcpath(), CoreSupport.srcpath(), DeviceSupport.srcpath()], defines=['STM32F10X_HD', 'USE_STDPERIPH_DRIVER'], install_path='${PREFIX}')
    bld(rule='${OBJCOPY} -O ihex ${SRC} ${TGT}', source='{}.elf'.format(APPNAME), target='{}.hex'.format(APPNAME), install_path='${PREFIX}')
    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}', source='{}.elf'.format(APPNAME), target='{}.bin'.format(APPNAME), install_path='${PREFIX}')


def download(ctx):
    download_file = ctx.bldnode.find_node('{}.hex'.format(APPNAME))
    if(download_file):
        ctx.exec_command('openocd -f {} -c init -c halt -c flash write_image erase {}'.format('openocd.cfg', download_file.bldpath()), cwd=ctx.bldnode)
    else:
        raise Errors.WafError(msg='Please build the project first !')


class Download(BuildContext):
    cmd = 'download'
    fun = 'download'


# Copy compile_commands.json to the project root
@TaskGen.feature('c', 'cxx')
@TaskGen.after_method('process_use', 'collect_compilation_db_tasks')
def post_collect_compilation_db_tasks(self):
    if(self.bld.env.CLANG_COMPILATION_DATABASE):
        self.bld.add_post_fun(copy_compile_commands)


def copy_compile_commands(ctx):
    src_file = ctx.bldnode.find_node('compile_commands.json')
    if(src_file):
        ctx.srcnode.make_node('compile_commands.json').write(src_file.read())
        Logs.info("Build commands will be stored in compile_commands.json")
