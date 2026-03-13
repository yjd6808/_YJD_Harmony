# Basic Rule

1. Answer using korean language.

# Workflow

When solving problems:

1. Analyze the existing code first.
2. Identify all files that need modification.
3. Create a modification plan.
4. Modify all required files.
5. Run the build once after all modifications are completed.

# Build Policy

- Do not run builds after each file modification.
- If multiple files are modified, complete all edits first.
- Run the build only once after all changes are finished.


# Coding Convention

All generated code must follow the rules below.

## 1. Naming Rules

### Private Member Variables
Applies to: non-static private members only (not public, not protected)

Rule:
- suffix "_"
- camelCase

Example:
health_
playerInfo_

---

### Parameter Variables

Pointer parameters:
- prefix "_p"
- camelCase

Example:
_pPlayer

Smart pointer parameters (type name ends with "Ptr"):
- prefix "_p"
- camelCase

Example:
_pPlayer

Non-pointer parameters:
- prefix "_"
- camelCase

Example:
_level

---

### Local Variables

Pointer local variables:
- prefix "p"
- camelCase

Example:
pPlayer

Smart pointer local variables (type name ends with "Ptr"):
- prefix "p"
- camelCase

Example:
pPlayer

Non-pointer local variables:
- camelCase

Example:
playerCount
attackSpeed

---

### constexpr Local Variables

Applies to:
- constexpr (not const)

Rule:
- UPPER_CASE

Example:
MAX_PLAYER_COUNT

---

### Forbidden Type Prefixes

The following prefixes must NOT be used in:
- local variables
- parameters
- private member variables

Forbidden prefixes:
dw
ui
i
v

Example (invalid):
iCount
dwSize

Example (valid):
count
size

---

## 2. Formatting Rules

### Indentation Style
Use Allman style.

Example:

if (condition)
{
    DoSomething();
}

---

### Single-line Control Statements

For the following statements:
return
continue
break

Braces {} may be omitted.

Example:

if (value == 0)
    return;

---

## 3. Function Visual Separators (.cpp only)

Each function must begin with a visual separator made of '/'.

Separator length rule:

separator_length = 90 - (indent_level × 4)

Examples:

Indent level 0:
//////////////////////////////////////////////////////////////////////////////////////////
void function()
{

}

Indent level 1:
namespace NA
{
    //////////////////////////////////////////////////////////////////////////////////////
    void function()
    {

    }
}
    

Indent level 2:
namespace NA
{
    //////////////////////////////////////////////////////////////////////////////////////
    class CA
    {
        //////////////////////////////////////////////////////////////////////////////////
        void function()
        {

        }
    }
}
        