#pragma once
#include "AuraWidgetVisibilityEvents.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraWidgetEventSignature, AActor*, Player);
