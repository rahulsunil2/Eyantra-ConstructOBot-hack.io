@ECHO ON
SET CURRENT_PATH=%cd%
SET VISUAL_PATH="C:\Program Files (x86)\Microsoft Visual Studio\Installer"
SET VREP_PATH="C:\Program Files\V-REP3\V-REP_PRO_EDU"

echo %CURRENT_PATH%

rm task_0_submission.txt
echo "Initiating VISUAL studio Version checking" >> task_0_submission.txt
echo "-------------------------------" >> task_0_submission.txt

cd %VISUAL_PATH%

vswhere.exe >> "%CURRENT_PATH%\task_0_submission.txt"

echo "Version checked sucessfully" >> "%CURRENT_PATH%\task_0_submission.txt"

echo "Initiating V-rep Checking" >> "%CURRENT_PATH%\task_0_submission.txt"

cd %VREP_PATH%

vrep.exe -h -s10 -q >> "%CURRENT_PATH%\task_0_submission.txt"

cd %CURRENT_PATH%
echo "Vrep Check complete" >> task_0_submission.txt
