dnl YAML_REQUIRE()
dnl Check for LibYAML
dnl If found, YAML_CPPFLAGS and YAML_LDFLAGS will be set.
AC_DEFUN([YAML_REQUIRE],
    [
    YAML_CPPFLAGS=
    dnl YAML_CPPFLAGS="-I/usr/local/include/yaml-cpp"
    YAML_LDFLAGS="-lyaml-cpp"
    AC_SUBST([YAML_CPPFLAGS])
    AC_SUBST([YAML_LDFLAGS])
    ])
