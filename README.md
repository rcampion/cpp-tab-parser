# cpp-tab-parser

C++ Tab Delimited File Parser using Flex/Bison

This project was created to parse a tab delimited contacts file that was created by
by a MS-SQL file export.

The MS-SQL file export did not produce a file that could be imported via LOAD DATA into MySQL.

The number of columns were not consistent in each row.

This project produces two files:

A SQL create script table and a SQL import data script.
