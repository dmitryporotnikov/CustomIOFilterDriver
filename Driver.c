#include <fltKernel.h>
#include <ntddk.h>
#include <ntifs.h>
#include <dontuse.h>
#include <suppress.h>
#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

PFLT_FILTER gFilterHandle;
FLT_PREOP_CALLBACK_STATUS PreOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
NTSTATUS UnloadDriver(FLT_FILTER_UNLOAD_FLAGS Flags);

const FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_READ, 0, PreOperationCallback, NULL },
    { IRP_MJ_OPERATION_END }
};

const FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION), // Size
    FLT_REGISTRATION_VERSION, // Version
    0, // Flags
    NULL, // Context
    Callbacks, // Operation callbacks
    UnloadDriver, // FilterUnload
    NULL, // InstanceSetup
    NULL, // InstanceQueryTeardown
    NULL, // InstanceTeardownStart
    NULL, // InstanceTeardownComplete
    NULL, NULL, NULL // Reserved
};

FLT_PREOP_CALLBACK_STATUS PreOperationCallback(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID* CompletionContext)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    // Check if the operation is initiated by user-mode
    if (Data->RequestorMode == UserMode) {
        PFLT_FILE_NAME_INFORMATION fileNameInfo;
        NTSTATUS status;
        UNICODE_STRING excludedPath = RTL_CONSTANT_STRING(L"\\Users\\");

        // Retrieve the normalized name information for the file
        status = FltGetFileNameInformation(Data,
            FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
            &fileNameInfo);
        if (NT_SUCCESS(status)) {
            status = FltParseFileNameInformation(fileNameInfo);

            if (NT_SUCCESS(status)) {
                // Check if the file path starts with "C:\Users" (accounting for device prefixes)
                if (wcsstr(fileNameInfo->Name.Buffer, excludedPath.Buffer) != NULL) {
                    // The file is in the C:\Users directory or subdirectories; skip delay
                    FltReleaseFileNameInformation(fileNameInfo);
                    return FLT_PREOP_SUCCESS_NO_CALLBACK;
                }
            }

            // Release the file name information after use
            FltReleaseFileNameInformation(fileNameInfo);
        }

        // If not excluded, introduce delay
        LARGE_INTEGER delay;
        delay.QuadPart = -(10 * 1000 * 1000 * (LONGLONG)(Data->Iopb->TargetFileObject->CurrentByteOffset.LowPart % 4));
        KeDelayExecutionThread(KernelMode, FALSE, &delay);
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;

    status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);
    if (NT_SUCCESS(status)) {
        // Start filtering I/O
        status = FltStartFiltering(gFilterHandle);
        if (!NT_SUCCESS(status)) {
            FltUnregisterFilter(gFilterHandle);
        }
    }

    return status;
}

NTSTATUS UnloadDriver(FLT_FILTER_UNLOAD_FLAGS Flags)
{
    UNREFERENCED_PARAMETER(Flags);

    FltUnregisterFilter(gFilterHandle);

    return STATUS_SUCCESS;
}
