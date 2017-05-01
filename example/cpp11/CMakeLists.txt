IF (MSGPACK_CXX11)
    INCLUDE_DIRECTORIES (
        ../include
    )

    LIST (APPEND exec_PROGRAMS
        container.cpp
        non_def_con_class.cpp
    )

    IF ("${MSGPACK_DEFAULT_API_VERSION}" GREATER "1")
        LIST (APPEND exec_PROGRAMS
            socket_stream_example.cpp
        )
    ENDIF ()

    FOREACH (source_file ${exec_PROGRAMS})
        GET_FILENAME_COMPONENT (source_file_we ${source_file} NAME_WE)
        ADD_EXECUTABLE (
            ${source_file_we}
            ${source_file}
        )
        TARGET_LINK_LIBRARIES (${source_file_we}
            MsgPack::msgpackcpp
        )
    ENDFOREACH ()
ENDIF ()
