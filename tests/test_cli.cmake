# End-to-end tests for the damm93 command-line tool.
# Run via ctest; expects -DDAMM93=<path to executable>.

function(expect_run expected_status expected_output)
    execute_process(
        COMMAND ${DAMM93} ${ARGN}
        RESULT_VARIABLE status
        OUTPUT_VARIABLE output
        ERROR_QUIET)
    string(REPLACE "\r" "" output "${output}")
    if(NOT status EQUAL expected_status)
        message(FATAL_ERROR "damm93 ${ARGN}: exit ${status}, expected ${expected_status}")
    endif()
    if(NOT "${expected_output}" STREQUAL "<any>" AND NOT output STREQUAL "${expected_output}")
        message(FATAL_ERROR "damm93 ${ARGN}: got\n${output}\nexpected\n${expected_output}")
    endif()
endfunction()

expect_run(0 "Base93 Number: World\nBase93 Check Digit: q\nError Check: 0\n" World)
expect_run(0 "Base93 Number: Worldq\nBase93 Check Digit: 0\nError Check: 1\n" Worldq)
expect_run(0 "Worldq\n" -a World)
expect_run(0 "" -c Worldq)
expect_run(1 "" -c Worldq Wordlq)
expect_run(0 "<any>" -a -- -leading-dash)
expect_run(1 "<any>" -a "bad\tchar")
expect_run(1 "Base93 Number: bad\tchar\nBase93 Check Digit: invalid character\nError Check: 0\n" "bad\tchar")
expect_run(0 "Hello_worldu\n" -a "Hello_world")
expect_run(1 "<any>" -a "Hello world")
expect_run(2 "<any>" --bogus)
expect_run(0 "<any>" --help)
