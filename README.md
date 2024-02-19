# CS204_Project
Risc-v 32 bit assembler
# note to collaborators
Add all the documentation for the project here, i.e. how to use etc.
while making and pushing a commit, always give a comment with what you did.
note that wsl in vscode terminal gives some glitches occasionally, use normal ubuntu terminal in that case.
write a message in this section to confirm if git is properly set up.
hello, harjas this side.
# note to users
---write the use documentation here for evaluation.
1) instructions.txt contains the list of supported instructions, with their type and number of arguments required.
2) test.asm is a test file used to check the working of the codes
3) to compile the syntax checker, use commands gcc check_file.cpp -o check_file
4) to run the code with test.asm, use command ./check_file test.asm

# improvements
* in the syntax checker, it does no check register or immediate operands for each instruction etc.
    - may make another check...something file to check for these. leave it for now.
# glitches
for a command like *add x3,x4,x5,* it says there are no errors in the syntax, even if there is a stray comma.