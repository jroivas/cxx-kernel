#add_custom_command(
#    PRE_BUILD
#    OUTPUT Python-${PYTHON_VERSION}
#    COMMAND wget "https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tar.xz"
#    COMMAND tar xf "Python-${PYTHON_VERSION}.tar.xz"
#    )
#
#add_custom_target(fetch_python ALL
#    DEPENDS Python-${PYTHON_VERSION}
#    )
