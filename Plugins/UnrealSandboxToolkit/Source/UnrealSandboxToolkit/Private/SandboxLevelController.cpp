#include "SandboxLevelController.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"

#include <string>
#include <vector>
#include <random>

// Глобальная карта объектов по идентификатору
TMap<int32, TSubclassOf<ASandboxObject>> ASandboxLevelController::ObjectMapById;
// Статический указатель на экземпляр контроллера уровня
ASandboxLevelController* ASandboxLevelController::StaticSelf;

// Конструктор класса ASandboxLevelController
ASandboxLevelController::ASandboxLevelController() {
    // Устанавливаем имя карты по умолчанию
    MapName = TEXT("World 0");
}

// Метод, вызываемый при начале игры
void ASandboxLevelController::BeginPlay() {
    // Вызываем родительский метод BeginPlay
    Super::BeginPlay();
    // Очищаем глобальную карту объектов
    GlobalObjectMap.Empty();
    // Подготавливаем метаданные
    PrepareMetaData();

    // Проверяем, установлен ли статический указатель на экземпляр контроллера
    if (!StaticSelf) {
        // TODO: добавить предупреждение
    }

    // Устанавливаем статический указатель на текущий экземпляр
    StaticSelf = this;
}

// Метод, вызываемый при завершении игры
void ASandboxLevelController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
    // Вызываем родительский метод EndPlay
    Super::EndPlay(EndPlayReason);
    // Сбрасываем статический указатель на экземпляр контроллера
    StaticSelf = nullptr;
}

// Метод, вызываемый каждый тик игры
void ASandboxLevelController::Tick(float DeltaTime) {
    // Вызываем родительский метод Tick
    Super::Tick(DeltaTime);
}

// Метод для сохранения объекта в JSON
void ASandboxLevelController::SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc) {
    // Начинаем запись объекта в JSON
    JsonWriter->WriteObjectStart();
    JsonWriter->WriteObjectStart("Object");

    // Записываем идентификаторы класса и типа, а также сетевой идентификатор
    JsonWriter->WriteValue("ClassId", ObjDesc.ClassId);
    JsonWriter->WriteValue("TypeId", ObjDesc.TypeId);
    JsonWriter->WriteValue("NetUid", ObjDesc.NetUid);

    // Записываем позицию объекта
    JsonWriter->WriteArrayStart("Location");
    JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().X);
    JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Y);
    JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Z);
    JsonWriter->WriteArrayEnd();

    // Записываем вращение объекта
    JsonWriter->WriteArrayStart("Rotation");
    JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Pitch);
    JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Yaw);
    JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Roll);
    JsonWriter->WriteArrayEnd();

    // Записываем масштаб объекта
    JsonWriter->WriteArrayStart("Scale");
    JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().X);
    JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().Y);
    JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().Z);
    JsonWriter->WriteArrayEnd();

    // Если у объекта есть контейнеры, записываем их
    if (ObjDesc.Container.Num() > 0) {
        JsonWriter->WriteArrayStart("Containers");

        JsonWriter->WriteObjectStart();
        JsonWriter->WriteObjectStart("Container");

        FString Name = GetName();
        JsonWriter->WriteValue("Name", TEXT("Container"));

        JsonWriter->WriteArrayStart("Content");

        for (const auto& TmpStack : ObjDesc.Container) {
            if (TmpStack.Stack.Amount > 0) {
                JsonWriter->WriteObjectStart();

                const ASandboxObject* SandboxObject = Cast<ASandboxObject>(TmpStack.Stack.GetObject());
                FString ClassName = SandboxObject->GetClass()->GetName();
                JsonWriter->WriteValue("SlotId", TmpStack.SlotId);
                JsonWriter->WriteValue("Class", ClassName);
                JsonWriter->WriteValue("ClassId", (int64)SandboxObject->GetSandboxClassId());
                JsonWriter->WriteValue("TypeId", SandboxObject->GetSandboxTypeId());
                JsonWriter->WriteValue("Amount", TmpStack.Stack.Amount);

                JsonWriter->WriteObjectEnd();
            }
        }
        JsonWriter->WriteArrayEnd();

        JsonWriter->WriteObjectEnd();
        JsonWriter->WriteObjectEnd();

        JsonWriter->WriteArrayEnd();
    }

    // Если у объекта есть свойства, записываем их
    if (ObjDesc.PropertyMap.Num() > 0) {
        JsonWriter->WriteObjectStart("Properties");
        for (auto& Itm : ObjDesc.PropertyMap) {
            JsonWriter->WriteValue(Itm.Key, Itm.Value);
        }
        JsonWriter->WriteObjectEnd();
    }

    JsonWriter->WriteObjectEnd();
    JsonWriter->WriteObjectEnd();
}

// Метод для сохранения подготовленных объектов в JSON файл
void ASandboxLevelController::SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) {
    // Логируем начало сохранения уровня
    UE_LOG(LogTemp, Log, TEXT("----------- save level json -----------"));

    FString JsonStr;
    FString FileName = TEXT("level.json");
    FString SavePath = FPaths::ProjectSavedDir();
    FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

    // Логируем путь к файлу JSON
    UE_LOG(LogTemp, Log, TEXT("level json path -> %s"), *FullPath);

    // Создаем JSON писатель
    TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);

    JsonWriter->WriteObjectStart();

    // Сохраняем расширенные данные уровня
    SaveLevelJsonExt(JsonWriter);

    // Записываем список объектов песочницы
    JsonWriter->WriteArrayStart("SandboxObjectList");

    for (const auto& ObjDesc : ObjDescList) {
        SaveObject(JsonWriter, ObjDesc);
    }

    JsonWriter->WriteArrayEnd();

    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    // Сохраняем строку JSON в файл
    FFileHelper::SaveStringToFile(*JsonStr, *FullPath);
}

// Метод для создания дескриптора объекта песочницы
FSandboxObjectDescriptor FSandboxObjectDescriptor::MakeObjDescriptor(ASandboxObject* SandboxObject) {
    FSandboxObjectDescriptor ObjDesc;
    ObjDesc.ClassId = SandboxObject->GetSandboxClassId();
    ObjDesc.TypeId = SandboxObject->GetSandboxTypeId();
    ObjDesc.Transform = SandboxObject->GetTransform();
    ObjDesc.PropertyMap = SandboxObject->PropertyMap;
    ObjDesc.NetUid = SandboxObject->GetSandboxNetUid();

    // Получаем контейнер объекта
    UContainerComponent* Container = SandboxObject->GetContainer(TEXT("ObjectContainer"));
    if (Container) {
        TArray<FContainerStack> Content = Container->Content;
        int SlotId = 0;
        for (const auto& Stack : Content) {
            if (Stack.Amount > 0) {
                if (Stack.GetObject()) {
                    FTempContainerStack TempContainerStack;
                    TempContainerStack.Stack = Stack;
                    TempContainerStack.SlotId = SlotId;
                    ObjDesc.Container.Add(TempContainerStack);
                }
            }

            SlotId++;
        }
    }

    return ObjDesc;
}

// Метод для подготовки объектов для сохранения
void ASandboxLevelController::PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList) {
    for (TActorIterator<ASandboxObject> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
        ASandboxObject* SandboxObject = Cast<ASandboxObject>(*ActorItr);
        if (SandboxObject) {
            if (SandboxObject->GetParentActor()) {
                continue;
            }

            FSandboxObjectDescriptor ObjDesc = FSandboxObjectDescriptor::MakeObjDescriptor(SandboxObject);
            ObjDescList.Add(ObjDesc);
        }
    }
}

// Метод для сохранения уровня в JSON
void ASandboxLevelController::SaveLevelJson() {
    TArray<FSandboxObjectDescriptor> ObjDescList;
    PrepareObjectForSave(ObjDescList);
    SavePreparedObjects(ObjDescList);
}

// Метод для сохранения расширенных данных уровня в JSON
void ASandboxLevelController::SaveLevelJsonExt(TSharedRef <TJsonWriter<TCHAR>> JsonWriter) {

}

// Метод для подготовки метаданных
void ASandboxLevelController::PrepareMetaData() {
    if (bIsMetaDataReady) {
        return;
    }

    if (!ObjectMap) {
        UE_LOG(LogTemp, Error, TEXT("ASandboxLevelController::LoadLevelJson() -----  ObjectMap == NULL"));
        return;
    }

    // Логируем начало загрузки карты объектов
    UE_LOG(LogTemp, Log, TEXT("----------- load object map -----------"));

    ObjectMapByClassName.Empty();
    ObjectMapById.Empty();

    // Копируем список объектов и сортируем его по идентификатору класса
    TArray<TSubclassOf<ASandboxObject>> ObjectListCopy = ObjectMap->ObjectList;
    ObjectListCopy.Sort([](const TSubclassOf<ASandboxObject>& ip1, const TSubclassOf<ASandboxObject>& ip2) {
        ASandboxObject* SandboxObject1 = Cast<ASandboxObject>(ip1->ClassDefaultObject);
        ASandboxObject* SandboxObject2 = Cast<ASandboxObject>(ip2->ClassDefaultObject);
        uint64 ClassId1 = SandboxObject1->GetSandboxClassId();
        uint64 ClassId2 = SandboxObject2->GetSandboxClassId();
        return  ClassId1 < ClassId2;
    });

    // Заполняем карты объектов по имени класса и идентификатору
    for (const TSubclassOf<ASandboxObject>& SandboxObjectSubclass : ObjectListCopy) {
        ASandboxObject* SandboxObject = Cast<ASandboxObject>(SandboxObjectSubclass->ClassDefaultObject);
        uint64 ClassId = SandboxObject->GetSandboxClassId();
        FString ClassName = SandboxObjectSubclass->ClassDefaultObject->GetClass()->GetName();

        if (ClassId == 0) {
            UE_LOG(LogTemp, Error, TEXT("ClassName -> %s has no class id"), *ClassName);
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("%s -> %d"), *ClassName, ClassId);
        ObjectMapByClassName.Add(ClassName, SandboxObjectSubclass);
        ObjectMapById.Add(ClassId, SandboxObjectSubclass);
    }

    bIsMetaDataReady = true;
}

// Метод для загрузки уровня из JSON
void ASandboxLevelController::LoadLevelJson() {
    if (!ObjectMap) {
        return;
    }

    PrepareMetaData();

    // Логируем начало загрузки уровня
    UE_LOG(LogTemp, Log, TEXT("----------- load level json -----------"));

    FString FileName = TEXT("level.json");
    FString SavePath = FPaths::ProjectSavedDir();
    FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

    FString JsonRaw;
    if (!FFileHelper::LoadFileToString(JsonRaw, *FullPath)) {
        UE_LOG(LogTemp, Error, TEXT("Error loading json file"));
    }

    TArray<FSandboxObjectDescriptor> ObjDescList;

    TSharedPtr<FJsonObject> JsonParsed;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonRaw);
    if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
        TArray <TSharedPtr<FJsonValue>> SandboxObjectList = JsonParsed->GetArrayField("SandboxObjectList");
        for (int Idx = 0; Idx < SandboxObjectList.Num(); Idx++) {
            TSharedPtr<FJsonObject> ObjPtr = SandboxObjectList[Idx]->AsObject();
            TSharedPtr<FJsonObject> SandboxObjectPtr = ObjPtr->GetObjectField(TEXT("Object"));

            FSandboxObjectDescriptor ObjDesc;

            ObjDesc.ClassId = SandboxObjectPtr->GetIntegerField(TEXT("ClassId"));
            ObjDesc.TypeId = SandboxObjectPtr->GetIntegerField(TEXT("TypeId"));
            ObjDesc.NetUid = SandboxObjectPtr->GetStringField(TEXT("NetUid"));

            FVector Location;
            TArray <TSharedPtr<FJsonValue>> LocationValArray = SandboxObjectPtr->GetArrayField("Location");
            Location.X = LocationValArray[0]->AsNumber();
            Location.Y = LocationValArray[1]->AsNumber();
            Location.Z = LocationValArray[2]->AsNumber();

            FRotator Rotation;
            TArray <TSharedPtr<FJsonValue>>  RotationValArray = SandboxObjectPtr->GetArrayField("Rotation");
            Rotation.Pitch = RotationValArray[0]->AsNumber();
            Rotation.Yaw = RotationValArray[1]->AsNumber();
            Rotation.Roll = RotationValArray[2]->AsNumber();

            FVector Scale;
            TArray <TSharedPtr<FJsonValue>> ScaleValArray = SandboxObjectPtr->GetArrayField("Scale");
            Scale.X = ScaleValArray[0]->AsNumber();
            Scale.Y = ScaleValArray[1]->AsNumber();
            Scale.Z = ScaleValArray[2]->AsNumber();

            ObjDesc.Transform = FTransform(Rotation, Location, Scale);

            const TSharedPtr<FJsonObject>* ObjPropJson = nullptr;
            bool bHasProperties = SandboxObjectPtr->TryGetObjectField("Properties", ObjPropJson);
            if (bHasProperties) {
                for (auto& Itm : (*ObjPropJson)->Values) {
                    FString Key = Itm.Key;
                    FString Value = Itm.Value->AsString();
                    ObjDesc.PropertyMap.Add(Key, Value);
                }
            }

            const TArray<TSharedPtr<FJsonValue>>* ContainersJsonArray = nullptr;
            bool bHasContainers = SandboxObjectPtr->TryGetArrayField("Containers", ContainersJsonArray);
            if (bHasContainers) {
                for (TSharedPtr<FJsonValue> ContainerJsonValue : *ContainersJsonArray) {
                    TSharedPtr<FJsonObject> ContainerJsonObject = ContainerJsonValue->AsObject()->GetObjectField(TEXT("Container"));
                    FString ContainerName = ContainerJsonObject->GetStringField("Name");
                    TArray<TSharedPtr<FJsonValue>> ContentArray = ContainerJsonObject->GetArrayField("Content");

                    for (TSharedPtr<FJsonValue> ContentJsonValue : ContentArray) {
                        int SlotId = ContentJsonValue->AsObject()->GetIntegerField("SlotId");
                        int ClassId = ContentJsonValue->AsObject()->GetIntegerField("ClassId");
                        int Amount = ContentJsonValue->AsObject()->GetIntegerField("Amount");

                        FContainerStack Stack;
                        Stack.Amount = Amount;
                        Stack.SandboxClassId = ClassId;

                        FTempContainerStack TempContainerStack;
                        TempContainerStack.Stack = Stack;
                        TempContainerStack.SlotId = SlotId;

                        ObjDesc.Container.Add(TempContainerStack);
                    }
                }
            }
            ObjDescList.Add(ObjDesc);
        }

        LoadLevelJsonExt(JsonParsed);
    }

    SpawnPreparedObjects(ObjDescList);
}

// Метод для создания подготовленного объекта
ASandboxObject* ASandboxLevelController::SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc) {
    ASandboxObject* NewObject = SpawnSandboxObject(ObjDesc.ClassId, ObjDesc.Transform, ObjDesc.NetUid);
    if (NewObject) {
        NewObject->PropertyMap = ObjDesc.PropertyMap;
        NewObject->PostLoadProperties();

        UContainerComponent* Container = NewObject->GetContainer(TEXT("ObjectContainer"));
        if (Container) {
            for (const auto& Itm : ObjDesc.Container) {
                Container->SetStackDirectly(Itm.Stack, Itm.SlotId);
            }
        }
    }

    return NewObject;
}

// Метод для создания подготовленных объектов
void ASandboxLevelController::SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) {
    for (const auto& ObjDesc : ObjDescList) {
        SpawnPreparedObject(ObjDesc);
    }
}

// Метод для генерации нового уникального идентификатора
FString ASandboxLevelController::GetNewUid() const {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    FString UID;

    do {
        uint64 uid1 = dis(gen);
        uint64 uid2 = dis(gen);
        uint64 uid3 = dis(gen);

        UID = FString::Printf(TEXT("%llx-%llx-%llx"), uid1, uid2, uid3);

    } while (GlobalObjectMap.Find(UID) != nullptr);

    return UID;
}

// Метод для создания объекта песочницы
ASandboxObject* ASandboxLevelController::SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid) {
    if (GetNetMode() != NM_Client) {
        TSubclassOf<ASandboxObject> SandboxObject = GetSandboxObjectByClassId(ClassId);
        if (SandboxObject) {
            UClass* SpawnClass = SandboxObject->ClassDefaultObject->GetClass();
            ASandboxObject* NewObject = (ASandboxObject*)GetWorld()->SpawnActor(SpawnClass, &Transform);
            if (NewObject) {
                NewObject->SandboxNetUid = (SandboxNetUid == "") ? GetNewUid() : SandboxNetUid;
                GlobalObjectMap.Add(NewObject->SandboxNetUid, NewObject);
            }
            return NewObject;
        }
    }

    return nullptr;
}

// Метод для загрузки расширенных данных уровня из JSON
void ASandboxLevelController::LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed) {

}

// Метод для получения объекта песочницы по идентификатору класса
TSubclassOf<ASandboxObject> ASandboxLevelController::GetSandboxObjectByClassId(int32 ClassId) {
    if (!bIsMetaDataReady) {
        PrepareMetaData();
    }

    if (ObjectMapById.Contains(ClassId)) {
        return ObjectMapById[ClassId];
    }

    return nullptr;
}

// Метод для получения объекта песочницы по умолчанию по идентификатору класса
ASandboxObject* ASandboxLevelController::GetDefaultSandboxObject(uint64 ClassId) {
    if (ObjectMapById.Contains(ClassId)) {
        return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject());
    }

    return nullptr;
}

// Метод для получения экземпляра контроллера уровня
ASandboxLevelController* ASandboxLevelController::GetInstance() {
    return StaticSelf;
}

// Метод для получения объекта песочницы по идентификатору класса
ASandboxObject* ASandboxLevelController::GetSandboxObject(uint64 ClassId) {
    if (!bIsMetaDataReady) {
        PrepareMetaData();
    }

    if (ObjectMapById.Contains(ClassId)) {
        return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject());
    }

    return nullptr;
}

// Метод для удаления объекта песочницы
bool ASandboxLevelController::RemoveSandboxObject(ASandboxObject* Obj) {
    if (GetNetMode() != NM_Client) {
        if (Obj) {
            FString NetUid = Obj->GetSandboxNetUid();
            Obj->Destroy();
            GlobalObjectMap.Remove(NetUid);
            return true;
        }
    }

    return false;
}

// Метод для получения объекта песочницы по сетевому идентификатору
ASandboxObject* ASandboxLevelController::GetObjectByNetUid(FString NetUid) {
    if (GlobalObjectMap.Contains(NetUid)) {
        return GlobalObjectMap[NetUid];
    }

    return nullptr;
}

// Метод для создания эффекта песочницы
ASandboxEffect* ASandboxLevelController::SpawnEffect(const int32 EffectId, const FTransform& Transform) {
    if (GetNetMode() != NM_Client) {
        if (ObjectMap->Effects.Contains(EffectId)) {
            TSubclassOf<ASandboxEffect> Effect = ObjectMap->Effects[EffectId];
            if (Effect) {
                UClass* SpawnClass = Effect->ClassDefaultObject->GetClass();
                return (ASandboxEffect*)GetWorld()->SpawnActor(SpawnClass, &Transform);
            }
        }
    }

    return nullptr;
}