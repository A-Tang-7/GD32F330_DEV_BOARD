::History file
del *.PcbLib.Zip /s
del *.SchLib.Zip /s
del *.PrjPcb.Zip /s
del *.PcbDoc.Zip /s
del *.SchDoc.Zip /s

::Project Logs file
del *.LOG /s

::Project Output file
del *.GPT /s
del *.apr /s
del *.APR_LIB /s
del *.csv /s
del *.drc /s
del *.DRR /s
del *.EXTREP /s
del *.GBL /s
del *.GBS /s
del *.GBO /s
del *.GBP /s
del *.GD1 /s
del *.GG1 /s
del *.GKO /s
del *.GM1 /s
del *.GM2 /s
del *.GM3 /s
del *.GM4 /s
del *.GM5 /s
del *.GM6 /s
del *.GM7 /s
del *.GM8 /s
del *.GM9 /s
del *.GM10 /s
del *.GM11 /s
del *.GM12 /s
del *.GM13 /s
del *.GM14 /s
del *.GM15 /s
del *.GP1 /s
del *.GP2 /s
del *.GPB /s
del *.GTL /s
del *.GTO /s
del *.GTP /s
del *.GTS /s
del *.html /s
del *.LDP /s
del *.REP /s
del *.RUL /s
del *.OutJob /s

::others file
del *.SchDocPreview /s
del *.PrjPcbStructure /s
del *.PrjPcb /s
del *.IntLib /s
del *.PcbLib /s
del *.SchLib /s
del *.step /s

:: delete empty file
for /f "tokens=*" %%a in ('dir /b /ad /s') do rd "%%a" 

exit