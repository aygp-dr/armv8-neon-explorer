#!/bin/bash
#!/bin/bash
# Script to download essential ARM documentation for offline reference

mkdir -p references

# ARM Architecture Reference Manual
echo "Downloading ARM Architecture Reference Manual..."
curl -L "https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Learn%20the%20Architecture/ARMv8-A%20Instruction%20Set%20Architecture.pdf" -o references/ARMv8-A_ISA.pdf

# ARM NEON Programmer's Guide
echo "Downloading NEON Programmer's Guide..."
curl -L "https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Learn%20the%20Architecture/Neon%20Programmer's%20Guide%20for%20Armv8-A.pdf" -o references/NEON_Programmers_Guide_Armv8-A.pdf

# ARM Compiler User Guide
echo "Downloading ARM Compiler User Guide..."
curl -L "https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Tools/ARM_Compiler_User_Guide.pdf" -o references/ARM_Compiler_User_Guide.pdf

# ARM Intrinsics Reference
echo "Downloading ARM NEON Intrinsics Reference..."
curl -L "https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Arm_NEON_Intrinsics_Reference.pdf" -o references/Arm_NEON_Intrinsics_Reference.pdf

echo "All references downloaded to 'references' directory."
