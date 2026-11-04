# End-to-end tests for the damm95 command-line tool.
# Run via ctest; expects -DDAMM95=<path to executable>.

function(expect_run expected_status expected_output)
    execute_process(
        COMMAND ${DAMM95} ${ARGN}
        RESULT_VARIABLE status
        OUTPUT_VARIABLE output
        ERROR_QUIET)
    string(REPLACE "\r" "" output "${output}")
    if(NOT status EQUAL expected_status)
        message(FATAL_ERROR "damm95 ${ARGN}: exit ${status}, expected ${expected_status}")
    endif()
    if(NOT "${expected_output}" STREQUAL "<any>" AND NOT output STREQUAL "${expected_output}")
        message(FATAL_ERROR "damm95 ${ARGN}: got\n${output}\nexpected\n${expected_output}")
    endif()
endfunction()

expect_run(0 "Base95 Number: Hello\nBase95 Check Digit: 8\nError Check: 0\n" Hello)
expect_run(0 "Base95 Number: Hello8\nBase95 Check Digit: 0\nError Check: 1\n" Hello8)
expect_run(0 "Hello8\n" -a Hello)
expect_run(0 "" -c Hello8)
expect_run(1 "" -c Hello8 Hellp8)
expect_run(0 "<any>" -a -- -leading-dash)
expect_run(1 "<any>" -a "bad\tchar")
expect_run(1 "Base95 Number: bad\tchar\nBase95 Check Digit: invalid character\nError Check: 0\n" "bad\tchar")
expect_run(0 "Hello world\\\n" -a "Hello world")
expect_run(2 "<any>" --bogus)
expect_run(0 "<any>" --help)
