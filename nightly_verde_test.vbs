
dim Body
dim Email
dim OShell
dim FSObject

' create the Shell, Email, and File Objects we need for this script
   set OShell = CreateObject("WScript.Shell")
   set Email = CreateObject("CDONTS.NewMail")
   set FSObject = CreateObject("Scripting.FileSystemObject")


' initialize the email
   Email.From="clinton@elemtech.com"
   'Email.To="clinton@elemtech.com"
   Email.To="verde-dev@sandia.gov"
   Email.Subject="VERDE test results for NT"
   Body="VERDE test results for NT" & Chr(13)

' re-compile verde
   'build_command = "cmd /C msdev verde\\verde_win\\verde.dsw /MAKE " & Chr(34) & "verde - Win32 Release" & Chr(34) & " > NTbuildresults"
   build_command = "cmd /C msdev verde\\verde_win\\verde.dsw /MAKE " & Chr(34) & "verde - Win32 Release" & Chr(34) & " /REBUILD > NTbuildresults"
   retval = OShell.Run (build_command, 1, TRUE)
   'Body = Body & Chr(13) & "buildresult: " & reval


' copy our executables where we need them
  exeexist = FSObject.FileExists( "verde\verde.exe" )
  comexist = FSObject.FileExists( "verde\verde.com" )


' if the files existed, then the build succeeded, proceed with tests
   if exeexist = 0 then
     Body = Body & Chr(13) & Chr(13) & "Build failed"
   elseif comexist = 0 then
     Body = Body & Chr(13) & Chr(13) & "Build failed"

   
   ' the build succeeded
   else

    'run the test
      testcommand = "cmd /C cd verde && tclsh83 verde_test all 2> ..\\NTtestresults"
      retval = OShell.Run ( testcommand, 1, TRUE )
   
    'if the tests failed, append the errors to the body of the email
      if retval > 0 then
         Body = Body & Chr(13)
   	     ' insert the stderr into the body
	     dim TxtStream
	     set TxtStream = FSObject.OpenTextFile( "NTtestresults", 1 )
	     Body = Body & TxtStream.ReadAll
         TxtStream.Close
         set TxtStream = nothing
      end if
   
     'append another note at the end of the email about the tests
      if retval > 0 then
         Body = Body & Chr(13) & Chr(13) & " tests failed"
      else
         Body = Body & Chr(13) & Chr(13) & "passed tests"
      end if

   end if

' for fun lets build debug as well
   OShell.Run "msdev verde\\verde_win\\verde.dsw /MAKE " & Chr(34) & "verde - Win32 Debug" & Chr(34)


' send the email
   Email.Body = Body
   Email.Send


' delete the objects that we allocated
   set Body = nothing
   set Email = nothing
   set OShell = nothing
   set FSObject = nothing