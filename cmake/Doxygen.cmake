# --------------------------------------------------------------------------
# Author:      Harrison Farrell
# Project:     Solo-Strategy Trading System
# Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
#
# Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
# This program is distributed WITHOUT ANY WARRANTY; without even the 
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
# --------------------------------------------------------------------------

find_package(Doxygen
	REQUIRED dot
	OPTIONAL_COMPONENTS mscgen dia
)

include(FetchContent)
FetchContent_Declare(
	doxygen-awesome-css
	GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
	GIT_TAG v2.3.3
	GIT_SHALLOW 1
)
FetchContent_MakeAvailable(doxygen-awesome-css)

function(Doxygen target input)
	set(NAME "doxygen-${target}")
	set(DOXYGEN_PROJECT_NAME          "Solo-Strategy")
	set(DOXYGEN_PROJECT_BRIEF         "High-Performance Low-Latency Trading System")
	set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${input}/Mainpage.md")
	set(DOXYGEN_EXTRACT_ALL           YES)
	set(DOXYGEN_EXTRACT_PRIVATE       YES)
	set(DOXYGEN_HTML_OUTPUT     ${PROJECT_BINARY_DIR}/doxygen/${name})
	set(DOXYGEN_GENERATE_HTML         YES)
	set(DOXYGEN_GENERATE_TREEVIEW     YES)
	set(DOXYGEN_HAVE_DOT              YES)
	set(DOXYGEN_DOT_IMAGE_FORMAT      svg)
	set(DOXYGEN_DOT_TRANSPARENT       YES)
	set(DOXYGEN_HTML_COLORSTYLE       DARK)
	set(DOXYGEN_HTML_EXTRA_STYLESHEET
		${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css)
	
	doxygen_add_docs(${NAME}
		${input}
		COMMENT "Generate HTML documentation"
	)
endfunction()