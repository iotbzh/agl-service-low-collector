###########################################################################
# Copyright 2018 IoT.bzh
#
# author: Sebastien Douheret <sebastien@iot.bzh>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.3)

PROJECT_TARGET_ADD(low-collector)

FILE(GLOB sourcelist "*.c")

if (USE_LIBCOLLECTD)
    FILE(GLOB collectd_glue_src "collectd_glue/*.c")

    FILE(GLOB collectd_src
        collectd/src/plugin.c
        collectd/src/daemon/common.c
        collectd/src/cpu.c
    )

	SET(sourcelist ${sourcelist} ${collectd_glue_src} ${collectd_src})
endif()

add_library(${TARGET_NAME} MODULE ${sourcelist})
target_link_libraries(${TARGET_NAME} ${link_libraries})

if (USE_LIBCOLLECTD)

	add_definitions(-DUSE_LIBCOLLECTD=1)

    target_compile_definitions(${TARGET_NAME}
        PUBLIC -DAGL_BINDING -DFP_LAYOUT_NEED_NOTHING
        PUBLIC -DHAVE_CONFIG_H
    )

    target_include_directories(${TARGET_NAME}
        PUBLIC collectd_glue/
        PUBLIC collectd/src
        PUBLIC collectd/src/daemon
    )

    # FIXME SEB - remove -Wno-error=implicit-function-declaration -Wno-implicit-function-declaration
    target_compile_options(${TARGET_NAME}
        PUBLIC -Wno-conversion -Wno-int-conversion
        PUBLIC -std=c99
        PUBLIC -Wno-error=implicit-function-declaration -Wno-implicit-function-declaration
    )

endif()


target_include_directories(${TARGET_NAME}
    PUBLIC src
)

add_custom_command(TARGET ${TARGET_NAME}
	PRE_BUILD
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/../package/htdocs
    COMMAND cp -rv ${CMAKE_CURRENT_SOURCE_DIR}/../htdocs ${CMAKE_CURRENT_BINARY_DIR}/../package/
)

set_target_properties(${TARGET_NAME} PROPERTIES
	PREFIX "afb-"
	LABELS "BINDING"
	LINK_FLAGS ${BINDINGS_LINK_FLAG}
    OUTPUT_NAME ${TARGET_NAME}
)

