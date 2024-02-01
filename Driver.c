#include <fltKernel.h>
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

FLT_PREOP_CALLBACK_STATUS PreOperationCallback(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    // Get a somewhat random seed based on the current system time
    LARGE_INTEGER currentTime;
    KeQuerySystemTime(&currentTime);

    // Use the lower bits of the current time as a simple "random" seed
    ULONG seed = currentTime.LowPart;

    // Generate a random delay between 0 and 5 seconds
    // The modulus operator helps ensure the delay is within the 0-5 range
    int delayInSeconds = seed % 6; // 0 to 5 inclusive

    LARGE_INTEGER delay;
    delay.QuadPart = -(10 * 1000 * 1000 * (LONGLONG)delayInSeconds); // Convert to 100-nanosecond intervals

    KeDelayExecutionThread(KernelMode, FALSE, &delay);

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
