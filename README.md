# WinHosts
# **WinHosts**

WinHosts is a Windows command-line utility designed to enhance system security and privacy by updating the hosts file to block ads, malicious sites, and other unwanted domains. This tool functions similarly to the Unix `hosts_updater.sh` script, providing a user-friendly interface to manage hosts entries on Windows systems.

---

## **Table of Contents**

- [Overview](#overview)
- [Features](#features)
- [Compilation](#compilation)
  - [Prerequisites](#prerequisites)
  - [Compilation Instructions](#compilation-instructions)
- [Usage](#usage)
  - [Running the Program](#running-the-program)
  - [Menu Options](#menu-options)
- [Important Notes](#important-notes)
- [License](#license)
- [Acknowledgments](#acknowledgments)

---

## **Overview**

WinHosts automates the process of updating the Windows hosts file to block a wide range of unwanted domains, including ads, phishing sites, and malware domains. By modifying the hosts file, these domains are redirected to `0.0.0.0`, effectively preventing your system from connecting to them.

---

## **Features**

- **User-Friendly Menu System**: Provides a simple menu interface to add/update or remove hosts entries.
- **Automated Hosts File Updates**:
  - Downloads the latest hosts file from a trusted source ([MVPS Hosts](https://winhelp2002.mvps.org/hosts.htm)).
  - Backs up the existing hosts file before making changes.
- **Precise Entry Management**:
  - Adds identifiable comments to the hosts file for easy removal of entries.
  - Ensures no duplicate entries are added.
- **DNS Cache Flush**:
  - Automatically flushes the DNS cache after modifying the hosts file to apply changes immediately.
- **Administrative Privilege Check**:
  - Verifies that the program is running with the necessary administrative rights.

---

## **Compilation**

### **Prerequisites**

To compile WinHosts, you need:

- **Microsoft Visual Studio Build Tools**:
  - Download from [Microsoft's official website](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022).
  - Install the **"Desktop development with C++"** workload.

### **Compilation Instructions**

1. **Download the Source Code**:

   - Save the `hosts_updater.cpp` file from this repository to your local machine.

2. **Open the Developer Command Prompt**:

   - **Start Menu** > **Visual Studio 2022** > **x64 Native Tools Command Prompt for VS 2022**.

3. **Navigate to the Source Code Directory**:

   ```cmd
   cd path\to\your\source\code
   ```

4. **Compile the Source Code**:

   ```cmd
   cl /EHsc /W4 hosts_updater.cpp
   ```

   - **Explanation of Flags**:
     - **`cl`**: Invokes the Microsoft C/C++ compiler.
     - **`/EHsc`**: Enables C++ exception handling.
     - **`/W4`**: Sets the warning level to 4 (informational warnings).
     - **`hosts_updater.cpp`**: The source file to compile.
     - **Libraries**:
       - The necessary libraries (`winhttp.lib`, `Advapi32.lib`) are linked within the code using `#pragma comment(lib, "...")`.

5. **Successful Compilation**:

   - If the compilation is successful, you will have `hosts_updater.exe` in your current directory.

---

## **Usage**

### **Running the Program**

- **Administrative Privileges Required**:

  - **Important**: You must run `hosts_updater.exe` as an administrator to modify the hosts file.

- **Steps to Run as Administrator**:

  1. Right-click on `hosts_updater.exe`.
  2. Select **"Run as administrator"**.
  3. If prompted by User Account Control (UAC), click **"Yes"**.

### **Menu Options**

Upon running the program, you'll see the following menu:

```
--------------------------------------------
Hosts File Updater Script
--------------------------------------------
1) Add/Update hosts entries
2) Remove hosts entries
3) Exit
--------------------------------------------
Please select an option [1-3]:
```

#### **Option 1: Add/Update Hosts Entries**

- **What It Does**:
  - Downloads the latest hosts file from MVPS.
  - Backs up the current hosts file to `C:\Windows\System32\drivers\etc\backups\`.
  - Appends new entries to the hosts file, enclosed within identifiable comments.

- **Steps**:

  1. **Select Option 1**.
  2. The program will display progress messages as it:
     - Creates a backup.
     - Downloads the hosts file.
     - Updates the hosts file.
  3. After completion, the DNS cache will be flushed automatically.

#### **Option 2: Remove Hosts Entries**

- **What It Does**:
  - Removes the entries added by this program from the hosts file.

- **Steps**:

  1. **Select Option 2**.
  2. The program will remove the specific entries between the markers.
  3. The DNS cache will be flushed automatically.

#### **Option 3: Exit**

- **What It Does**:
  - Exits the program.

---

## **Important Notes**

- **Backup Files**:
  - Backups are stored in `C:\Windows\System32\drivers\etc\backups\`.
  - Each backup file is timestamped for easy identification.

- **DNS Cache**:
  - The program flushes the DNS cache after modifying the hosts file.
  - This ensures changes take effect immediately.

- **Trusted Source**:
  - Hosts entries are downloaded from [MVPS Hosts](https://winhelp2002.mvps.org/hosts.htm), a reputable source for blocking malicious domains.

- **Administrative Rights**:
  - The program checks for administrative privileges.
  - If not running as administrator, it will display an error and exit.

- **Safety and Security**:
  - Modifying the hosts file can affect network connectivity.
  - Ensure you understand the changes being made and trust the source of the hosts file.

---

## **License**

This project is licensed under the MIT License.

```
MIT License

[Full MIT License Text]
```

---

## **Acknowledgments**

- **MVPS Hosts**: For providing a comprehensive hosts file to block unwanted domains.
- **Microsoft WinHTTP API**: For HTTP functionalities used in downloading the hosts file.
- **Microsoft Visual Studio**: For the development tools.

---

**Disclaimer**: Use this tool responsibly. Always ensure that you have backups of critical system files and understand the implications of modifying the hosts file.

---

**Contact**: For questions or support, please open an issue on the repository or contact [Your Email].

---

**Note**: Always review and understand scripts or programs before running them, especially when they require administrative privileges and modify system files.
