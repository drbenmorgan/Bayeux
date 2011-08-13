#!/usr/bin/env bash 
# -*- mode: shell-script; -*- 
# tests/run_tests.bash

APPNAME="tests/run_tests.bash"

opwd=$(pwd)
	
function my_exit ()
{
    cd ${opwd}
    exit $1
}

appname=${APPNAME} 
appversion=0.1 

the_base_dir=$(pwd)

# uncomment for usage related to a pkgtools managed package:
if [ "x${GEOMTOOLS_ROOT}" = "x" ]; then
    echo " ERROR: ${appname}: GEOMTOOLS_ROOT is not defined ! Abort !" >&2  
    my_exit 1
else
    if [ ! -d ${GEOMTOOLS_ROOT} ]; then
	echo " ERROR: ${appname}: GEOMTOOLS_ROOT directory does not exist ! Abort !" >&2  
	my_exit 1
    fi
    the_base_dir=${GEOMTOOLS_ROOT}
fi

libpkgtools_sourced=0

pkgtools_root=${PKGTOOLS_ROOT}
if [ ${libpkgtools_sourced} -eq 0 ]; then
    if [ "x${pkgtools_root}" != "x" ]; then
	if [ -f ${pkgtools_root}/lib/libpkgtools.sh ]; then
	    source ${pkgtools_root}/lib/libpkgtools.sh
	    libpkgtools_sourced=1
	fi
    fi
fi

if [ ${libpkgtools_sourced} -eq 0 ]; then
    libpkgtools_sh=${the_base_dir}/pkgtools.d/libpkgtools.sh 
    if [ -f ${libpkgtools_sh} ]; then
	source ${libpkgtools_sh}    
	libpkgtools_sourced=1
    fi
fi

if [ ${libpkgtools_sourced} -eq 0 ]; then
    echo " ERROR: ${appname}: Cannot find any 'libpkgtools.sh' setup file ! Abort !" >&2  
    my_exit 1
fi

pkgtools__msg_not_using_quiet
pkgtools__msg_not_using_verbose
pkgtools__msg_not_using_debug
pkgtools__msg_not_using_devel
pkgtools__msg_not_using_date
pkgtools__msg_using_color
pkgtools__ui_interactive
pkgtools__ui_not_using_gui

tmp_test_dir=${GEOMTOOLS_ROOT}/tests/tmp

function print_usage ()
{
    cat <<EOF

  ${appname} -- Description

  Usage:

    ${appname} [Options...] ACTION [Options...]

  Options:

    -h 
    --help    : print this help then exit

    --version  : print version then exit

    -d 
    --debug   : print debug mode informations

    -D 
    --devel   : print devel mode informations

    -v 
    --verbose : verbose print

    -i      
    --interactive  : run in interactive mode (this is the default)

    -b      
    --batch        : run in batch mode (no user interaction, disable GUI features)

    --no-color     : use un-colored message output

    --gui          : use GUI user interface (interactive mode only)

  Actions:

    run : Run the test programs

    clean : Clean the test programs' output

  Examples:

    shell> ${appname} --help
 
EOF
    return 0
}

#######################################################

the_action_mode=
the_action_options=
parse_switch=1
while [ -n "$1" ]; do
    token="$1"
    if [ "${token:0:1}" = "-" ]; then
	opt=${token}
	if [ ${parse_switch} -eq 0 ]; then
	    break
	fi
	if [ "${opt}" = "-h" -o "${opt}" = "--help" ]; then
	    print_usage
	    my_exit 0
	elif [ "${opt}" = "--version" ]; then
	    echo ${appversion}
	    my_exit 0
	elif [ "${opt}" = "-d" -o "${opt}" = "--debug" ]; then
	    pkgtools__msg_using_debug
	elif [ "${opt}" = "-D" -o "${opt}" = "--devel" ]; then
	    pkgtools__msg_using_devel
	elif [ "${opt}" = "-v" -o "${opt}" = "--verbose" ]; then
	    pkgtools__msg_using_verbose
	elif [ "${opt}" = "-i" -o "${opt}" = "--interactive" ]; then
	    pkgtools__ui_interactive
	elif [ "${opt}" = "-b" -o "${opt}" = "--batch" ]; then
	    pkgtools__ui_batch
	elif [ "${opt}" = "--no-color" ]; then
	    pkgtools__msg_not_using_color
	elif [ "${opt}" = "-G" -o "${opt}" = "--gui" ]; then
	    pkgtools__ui_using_gui
	    pkgtools__msg_devel "Using GUI" 	
	else
	    pkgtools__msg_error "Invalid option '${opt}' !" 
	    my_exit 1
	    # pkgtools__msg_warning "Ignoring option '${opt}' !" 
	fi
    else
	arg=${token}
	parse_switch=0
	if [ "x${the_action_mode}" = "x" ]; then
	    if [ "$arg" = "run" ]; then
		the_action_mode="${arg}"	
            elif [ "$arg" = "clean" ]; then
	        the_action_mode="${arg}"	
	    else
		pkgtools__msg_error "Invalid argument !"
		my_exit 1
	    fi
	else
	    # remaining arguments from command line are kept in "$@"
	    pkgtools__msg_devel "BREAK !"
	    break
	fi
    fi
    shift 1
done

the_action_options="$@"
pkgtools__msg_devel "the_action_mode=${the_action_mode}"
pkgtools__msg_devel "the_action_options=${the_action_options}"

# Batch mode:
pkgtools__ui_is_interactive
if [ $? -ne 0 ]; then
    pkgtools__ui_not_using_gui
fi

##########################################################

test_exe="\
test_address_set \
test_box \
test_circle \
test_color \
test_cylinder \
test_disk \
test_dummy_geomtools \
test_gdml_writer \
test_geom_id \
test_geomtools \
test_gnuplot_draw \
test_helix \
test_hexagon_box \
test_id_mgr \
test_id_selector \
test_i_model \
test_physical_volume \
test_placement_2 \
test_placement_3 \
test_placement \
test_polycone_2 \
test_polycone \
test_polyhedra \
test_polyline_3d \
test_rectangle \
test_regular_polygon \
test_rotation_3d \
test_serializable_2 \
test_serializable \
test_sphere \
test_subtraction_3d \
test_tessellated_solid \
test_tube \
test_utils \
test_model_factory \
"

### test_multiple_placement 
### test_intersection_3d 
### test_any_shape_3d 
### test_gnuplot_i 
### test_model_factory 
### test_intersection 
### test_union_3d 
### test_utils_2 
### test_regular_grid_placement 
### test_regular_linear_placement 
### test_logical_volume 

function do_clean ()
{
    __pkgtools__at_function_enter do_clean

    if [ -d ${tmp_test_dir} ]; then
	rm -fr ${tmp_test_dir}
    fi

    __pkgtools__at_function_exit
}

function do_run ()
{
    __pkgtools__at_function_enter do_run

    opwd=$(pwd)
    pkgtools__msg_notice "First clean the test temporary directory..."
    do_clean
    if [ ! -d ${tmp_test_dir} ]; then
	mkdir ${tmp_test_dir}
    fi
    cd ${tmp_test_dir}

    cat >> tests.log<<EOF
****************************************************
Starting tests...
****************************************************
EOF
    count=0
    error_count=0
    missing_count=0
    for exe in ${test_exe} ; do
	let count=count+1
	pkgtools__msg_notice "Running ${exe}..."
	cat >> tests.log<<EOF

****************************************************
Running ${exe}...
****************************************************
EOF
	bin=${GEOMTOOLS_BIN_DIR}/${exe}
	if [ ! -x ${bin} ]; then
	    pkgtools__msg_warning "No '${bin}' exectuable avalaible ! Please first build it !"
	    let missing_count=missing_count+1
	    continue
	fi
	if [ "${exe}" = "test_address_set" ]; then
	    echo "{34;44;54}" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_geom_id" ]; then
	    echo "[3:1.4.5]" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_tube" ]; then
	    echo "{tube 900 1000 1000}" | ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	elif [ "${exe}" = "test_model_factory" ]; then
	    echo world | ${bin} -D >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	else
	    ${bin} >> tests.log 2>&1
	    if [ $? -ne 0 ]; then
		let error_count=error_count+1
		pkgtools__msg_notice "${exe} failed !"
	    fi 
	fi
    done
 
    pkgtools__msg_notice "Log file is '$(pwd)/tests.log'"
    pkgtools__msg_notice "Total Number of executable   = ${count}"
    pkgtools__msg_notice "Number of missing executable = ${missing_count}"
    pkgtools__msg_notice "Number of failed executable  = ${error_count}"
     
    cat >> tests.log<<EOF

****************************************************
Tests stop here.
****************************************************
EOF
  
    cd ${opwd}

    __pkgtools__at_function_exit
    return 0
}

# function do_something_else ()
# {
#     __pkgtools__at_function_enter do_something_else
#     pkgtools__msg_warning "Nothing special !"
#     __pkgtools__at_function_exit
#     return 0
# }

function main ()
{
    __pkgtools__at_function_enter main

    local action_mode=${the_action_mode}
    local action_options=${the_action_options}

    # Some checks...
    if [ -z "${action_mode}" ]; then
	pkgtools__msg_error "Missing action !"
	print_usage
	__pkgtools__at_function_exit
	return 1
    fi

    # Perform action...    
    if [ "${action_mode}" = "run" ]; then
	do_run $@
	if [ $? -ne 0 ]; then
	    pkgtools__msg_error "Running failed !"
	    __pkgtools__at_function_exit
	    return 1
	fi
    fi
    
    if [ "${action_mode}" = "clean" ]; then
    	do_clean $@
    	if [ $? -ne 0 ]; then
    	    pkgtools__msg_error "Cleaning failed !"
    	    __pkgtools__at_function_exit
    	    return 1
    	fi
    fi
  
    __pkgtools__at_function_exit
    return 0
}

##########################################################

main 
if [ $? -ne 0 ]; then
    pkgtools__msg_error "Failure !"
    my_exit 1
fi
my_exit 0

# end of tests/run_tests.bash
