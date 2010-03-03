# This project file will not be modified by the Qt Creator IDE

CONFIG(debug, debug|release) {
# debug
} else {
# release
    # remove assert() functions
    DEFINES += NDEBUG
}