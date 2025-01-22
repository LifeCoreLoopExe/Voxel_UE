#include "SandboxLevelController.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"

#include <string>
#include <vector>
#include <random>

// Статические переменные класса для хранения карты объектов и указателя на себя
TMap<int32, TSubclassOf<ASandboxObject>> ASandboxLevelController::ObjectMapById;
ASandboxLevelController* ASandboxLevelController::StaticSelf;

// Конструктор - задаёт начальное имя мира
ASandboxLevelController::ASandboxLevelController() {
	MapName = TEXT("World 0");
}

// Вызывается при старте игры
void ASandboxLevelController::BeginPlay() {
	Super::BeginPlay();
	// Очищаем карту глобальных объектов
	GlobalObjectMap.Empty();
	// Подготавливаем метаданные
	PrepareMetaData();

	if (!StaticSelf) {
		// TODO warning
	}

	StaticSelf = this;
}

// Вызывается при завершении игры
void ASandboxLevelController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	StaticSelf = nullptr;
}

// Вызывается каждый кадр
void ASandboxLevelController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Сохраняет информацию об объекте в JSON формате
void ASandboxLevelController::SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc) {
	// Начинаем запись объекта
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteObjectStart("Object");

	// Записываем основные параметры объекта
	JsonWriter->WriteValue("ClassId", ObjDesc.ClassId);
	JsonWriter->WriteValue("TypeId", ObjDesc.TypeId);
	JsonWriter->WriteValue("NetUid", ObjDesc.NetUid);

	// Записываем позицию объекта
	JsonWriter->WriteArrayStart("Location");
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().X);
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Y);
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Z);
	JsonWriter->WriteArrayEnd();

	// Записываем поворот объекта
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

	// Если у объекта есть контейнер с предметами - сохраняем его содержимое
	if (ObjDesc.Container.Num() > 0) {
		JsonWriter->WriteArrayStart("Containers");
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteObjectStart("Container");

		FString Name = GetName();
		JsonWriter->WriteValue("Name", TEXT("Container"));

		JsonWriter->WriteArrayStart("Content");

		// Сохраняем каждый предмет в контейнере
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

	// Сохраняем дополнительные свойства объекта
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

// Сохраняет список подготовленных объектов в JSON файл
void ASandboxLevelController::SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) {
	UE_LOG(LogTemp, Log, TEXT("----------- save level json -----------"));

	// Формируем путь к файлу сохранения
	FString JsonStr;
	FString FileName = TEXT("level.json");
	FString SavePath = FPaths::ProjectSavedDir();
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

	UE_LOG(LogTemp, Log, TEXT("level json path -> %s"), *FullPath);

	// Создаём JSON писателя
	TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);

	JsonWriter->WriteObjectStart();

	// Сохраняем дополнительные данные уровня
	SaveLevelJsonExt(JsonWriter);

	// Сохраняем список объектов
	JsonWriter->WriteArrayStart("SandboxObjectList");

	for (const auto& ObjDesc : ObjDescList) {
		SaveObject(JsonWriter, ObjDesc);
	}

	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	// Сохраняем JSON в файл
	FFileHelper::SaveStringToFile(*JsonStr, *FullPath);
}

// Создаёт описание объекта для сохранения
FSandboxObjectDescriptor FSandboxObjectDescriptor::MakeObjDescriptor(ASandboxObject* SandboxObject) {
	FSandboxObjectDescriptor ObjDesc;
	// Копируем основные параметры объекта
	ObjDesc.ClassId = SandboxObject->GetSandboxClassId();
	ObjDesc.TypeId = SandboxObject->GetSandboxTypeId();
	ObjDesc.Transform = SandboxObject->GetTransform();
	ObjDesc.PropertyMap = SandboxObject->PropertyMap;
	ObjDesc.NetUid = SandboxObject->GetSandboxNetUid();

	// Если у объекта есть контейнер - сохраняем его содержимое
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

// Подготавливает объекты для сохранения
void ASandboxLevelController::PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList) {
	// Проходим по всем объектам в мире
	for (TActorIterator<ASandboxObject> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ASandboxObject* SandboxObject = Cast<ASandboxObject>(*ActorItr);
		if (SandboxObject) {
			// Пропускаем объекты, у которых есть родитель
			if (SandboxObject->GetParentActor()) {
				continue;
			}

			// Создаём описание объекта и добавляем в список
			FSandboxObjectDescriptor ObjDesc = FSandboxObjectDescriptor::MakeObjDescriptor(SandboxObject);
			ObjDescList.Add(ObjDesc);
		}
	}
}

// Сохраняет уровень в JSON
void ASandboxLevelController::SaveLevelJson() {
	TArray<FSandboxObjectDescriptor> ObjDescList;
	PrepareObjectForSave(ObjDescList);
	SavePreparedObjects(ObjDescList);
}

// Дополнительное сохранение данных уровня (может быть переопределено)
void ASandboxLevelController::SaveLevelJsonExt(TSharedRef <TJsonWriter<TCHAR>> JsonWriter) {

}

// Подготавливает метаданные объектов
void ASandboxLevelController::PrepareMetaData() {
	if (bIsMetaDataReady) {
		return;
	}

	if (!ObjectMap) {
		UE_LOG(LogTemp, Error, TEXT("ASandboxLevelController::LoadLevelJson() -----  ObjectMap == NULL"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("----------- load object map -----------"));

	// Очищаем карты объектов
	ObjectMapByClassName.Empty();
	ObjectMapById.Empty();

	// Сортируем список объектов по ClassId
	TArray<TSubclassOf<ASandboxObject>> ObjectListCopy = ObjectMap->ObjectList;
	ObjectListCopy.Sort([](const TSubclassOf<ASandboxObject>& ip1, const TSubclassOf<ASandboxObject>& ip2) {
		ASandboxObject* SandboxObject1 = Cast<ASandboxObject>(ip1->ClassDefaultObject);
		ASandboxObject* SandboxObject2 = Cast<ASandboxObject>(ip2->ClassDefaultObject);
		uint64 ClassId1 = SandboxObject1->GetSandboxClassId();
		uint64 ClassId2 = SandboxObject2->GetSandboxClassId();
		return  ClassId1 < ClassId2;
	});

	// Заполняем карты объектов
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

// Загружает уровень из JSON файла
void ASandboxLevelController::LoadLevelJson() {
	if (!ObjectMap) {
		return;
	}

	PrepareMetaData();

	UE_LOG(LogTemp, Log, TEXT("----------- load level json -----------"));

	// Формируем путь к файлу
	FString FileName = TEXT("level.json");
	FString SavePath = FPaths::ProjectSavedDir();
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

	// Загружаем JSON файл
	FString JsonRaw;
	if (!FFileHelper::LoadFileToString(JsonRaw, *FullPath)) {
		UE_LOG(LogTemp, Error, TEXT("Error loading json file"));
	}

	TArray<FSandboxObjectDescriptor> ObjDescList;

	// Парсим JSON
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {
		// Читаем список объектов
		TArray <TSharedPtr<FJsonValue>> SandboxObjectList = JsonParsed->GetArrayField("SandboxObjectList");
		for (int Idx = 0; Idx < SandboxObjectList.Num(); Idx++) {
			TSharedPtr<FJsonObject> ObjPtr = SandboxObjectList[Idx]->AsObject();
			TSharedPtr<FJsonObject> SandboxObjectPtr = ObjPtr->GetObjectField(TEXT("Object"));

			FSandboxObjectDescriptor ObjDesc;

			// Читаем основные параметры объекта
			ObjDesc.ClassId = SandboxObjectPtr->GetIntegerField(TEXT("ClassId"));
			ObjDesc.TypeId = SandboxObjectPtr->GetIntegerField(TEXT("TypeId"));
			ObjDesc.NetUid = SandboxObjectPtr->GetStringField(TEXT("NetUid"));

			// Читаем позицию
			FVector Location;
			TArray <TSharedPtr<FJsonValue>> LocationValArray = SandboxObjectPtr->GetArrayField("Location");
			Location.X = LocationValArray[0]->AsNumber();
			Location.Y = LocationValArray[1]->AsNumber();
			Location.Z = LocationValArray[2]->AsNumber();

			// Читаем поворот
			FRotator Rotation;
			TArray <TSharedPtr<FJsonValue>>  RotationValArray = SandboxObjectPtr->GetArrayField("Rotation");
			Rotation.Pitch = RotationValArray[0]->AsNumber();
			Rotation.Yaw = RotationValArray[1]->AsNumber();
			Rotation.Roll = RotationValArray[2]->AsNumber();

			// Читаем масштаб
			FVector Scale;
			TArray <TSharedPtr<FJsonValue>> ScaleValArray = SandboxObjectPtr->GetArrayField("Scale");
			Scale.X = ScaleValArray[0]->AsNumber();
			Scale.Y = ScaleValArray[1]->AsNumber();
			Scale.Z = ScaleValArray[2]->AsNumber();

			ObjDesc.Transform = FTransform(Rotation, Location, Scale);

			// Читаем дополнительные свойства
			const TSharedPtr<FJsonObject>* ObjPropJson = nullptr;
			bool bHasProperties = SandboxObjectPtr->TryGetObjectField("Properties", ObjPropJson);
			if (bHasProperties) {
				for (auto& Itm : (*ObjPropJson)->Values) {
					FString Key = Itm.Key;
					FString Value = Itm.Value->AsString();
					ObjDesc.PropertyMap.Add(Key, Value);
				}
			}

			// Читаем содержимое контейнеров
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

	// Создаём объекты в мире
	SpawnPreparedObjects(ObjDescList);
}

// Создаёт подготовленный объект в мире
ASandboxObject* ASandboxLevelController::SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc) {
	ASandboxObject* NewObject = SpawnSandboxObject(ObjDesc.ClassId, ObjDesc.Transform, ObjDesc.NetUid);
	if (NewObject) {
		// Устанавливаем свойства объекта
		NewObject->PropertyMap = ObjDesc.PropertyMap;
		NewObject->PostLoadProperties();

		// Заполняем контейнер, если он есть
		UContainerComponent* Container = NewObject->GetContainer(TEXT("ObjectContainer"));
		if (Container) {
			for (const auto& Itm : ObjDesc.Container) {
				Container->SetStackDirectly(Itm.Stack, Itm.SlotId);
			}
		}
	}

	return NewObject;
}

// Создаёт список подготовленных объектов
void ASandboxLevelController::SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) {
	for (const auto& ObjDesc : ObjDescList) {
		SpawnPreparedObject(ObjDesc);
	}
}

// Генерирует новый уникальный идентификатор
FString ASandboxLevelController::GetNewUid() const {
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	FString UID;

	// Генерируем UID пока не получим уникальный
	do {
		uint64 uid1 = dis(gen);
		uint64 uid2 = dis(gen);
		uint64 uid3 = dis(gen);

		UID = FString::Printf(TEXT("%llx-%llx-%llx"), uid1, uid2, uid3);

	} while (GlobalObjectMap.Find(UID) != nullptr);

	return UID;
}

// Создаёт объект в мире
ASandboxObject* ASandboxLevelController::SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid) {
	if (GetNetMode() != NM_Client) {
		TSubclassOf<ASandboxObject> SandboxObject = GetSandboxObjectByClassId(ClassId);
		if (SandboxObject) {
			UClass* SpawnClass = SandboxObject->ClassDefaultObject->GetClass();
			ASandboxObject* NewObject = (ASandboxObject*)GetWorld()->SpawnActor(SpawnClass, &Transform);
			if (NewObject) {
				// Генерируем или устанавливаем UID и добавляем в глобальную карту
				NewObject->SandboxNetUid = (SandboxNetUid == "") ? GetNewUid() : SandboxNetUid;
				GlobalObjectMap.Add(NewObject->SandboxNetUid, NewObject);
			}
			return NewObject;
		}
	}

	return nullptr;
}

// Дополнительная загрузка данных уровня (может быть переопределено)
void ASandboxLevelController::LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed) {

}

// Получает класс объекта по его ClassId
TSubclassOf<ASandboxObject> ASandboxLevelController::GetSandboxObjectByClassId(int32 ClassId) {
	if (!bIsMetaDataReady) {
		PrepareMetaData();
	}

	if (ObjectMapById.Contains(ClassId)) {
		return ObjectMapById[ClassId];
	}

	return nullptr;
}

// Получает объект по умолчанию для заданного ClassId
ASandboxObject* ASandboxLevelController::GetDefaultSandboxObject(uint64 ClassId) {
	if (ObjectMapById.Contains(ClassId)) {
		return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject());
	}

	return nullptr;
}

// Получает экземпляр контроллера уровня
ASandboxLevelController* ASandboxLevelController::GetInstance() {
	return StaticSelf;
}

// Получает объект по его ClassId
ASandboxObject* ASandboxLevelController::GetSandboxObject(uint64 ClassId) {
	if (!bIsMetaDataReady) {
		PrepareMetaData();
	}

	if (ObjectMapById.Contains(ClassId)) {
		return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject());
	}

	return nullptr;
}

// Удаляет объект из мира
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

// Получает объект по его сетевому UID
ASandboxObject* ASandboxLevelController::GetObjectByNetUid(FString NetUid) {
	if (GlobalObjectMap.Contains(NetUid)) {
		return GlobalObjectMap[NetUid];
	}

	return nullptr;
}

// Создаёт эффект в мире
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