#include "SandboxLevelController.h" // Подключение заголовочного файла контроллера уровня песочницы
#include "SandboxObject.h" // Подключение заголовочного файла объекта песочницы
#include "ContainerComponent.h" // Подключение заголовочного файла компонента контейнера

#include <string> // Подключение библиотеки строк
#include <vector> // Подключение библиотеки векторов
#include <random> // Подключение библиотеки генерации случайных чисел

TMap<int32, TSubclassOf<ASandboxObject>> ASandboxLevelController::ObjectMapById; // Карта объектов по ID
ASandboxLevelController* ASandboxLevelController::StaticSelf; // Статическая ссылка на экземпляр контроллера уровня

ASandboxLevelController::ASandboxLevelController() { // Конструктор контроллера уровня
	MapName = TEXT("World 0"); // Инициализация имени карты
}

void ASandboxLevelController::BeginPlay() { // Метод, вызываемый при начале игры
	Super::BeginPlay(); // Вызов метода базового класса
	GlobalObjectMap.Empty(); // Очистка глобальной карты объектов
	PrepareMetaData(); // Подготовка метаданных

	if (!StaticSelf) { 
		// TODO предупреждение
	}

	StaticSelf = this; // Установка статической ссылки на текущий экземпляр
}

void ASandboxLevelController::EndPlay(const EEndPlayReason::Type EndPlayReason) { // Метод, вызываемый при окончании игры
	Super::EndPlay(EndPlayReason); // Вызов метода базового класса
	StaticSelf = nullptr; // Обнуление статической ссылки
}

void ASandboxLevelController::Tick(float DeltaTime) { // Метод, вызываемый каждый кадр
	Super::Tick(DeltaTime); // Вызов метода базового класса
}

void ASandboxLevelController::SaveObject(TSharedRef <TJsonWriter<TCHAR>> JsonWriter, const FSandboxObjectDescriptor& ObjDesc) { // Метод для сохранения объекта в JSON
	JsonWriter->WriteObjectStart(); // Начало записи объекта
	JsonWriter->WriteObjectStart("Object"); // Начало записи объекта "Object"

	//JsonWriter->WriteValue("Name", SandboxObjectName); // Запись имени объекта (закомментировано)
	//JsonWriter->WriteValue("Class", Name); // Запись класса объекта (закомментировано)
	JsonWriter->WriteValue("ClassId", ObjDesc.ClassId); // Запись ID класса
	JsonWriter->WriteValue("TypeId", ObjDesc.TypeId); // Запись ID типа
	JsonWriter->WriteValue("NetUid", ObjDesc.NetUid); // Запись сетевого UID

	JsonWriter->WriteArrayStart("Location"); // Начало записи массива "Location"
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().X); // Запись координаты X
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Y); // Запись координаты Y
	JsonWriter->WriteValue(ObjDesc.Transform.GetLocation().Z); // Запись координаты Z
	JsonWriter->WriteArrayEnd(); // Конец записи массива

	JsonWriter->WriteArrayStart("Rotation"); // Начало записи массива "Rotation"
	JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Pitch); // Запись угла поворота Pitch
	JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Yaw); // Запись угла поворота Yaw
	JsonWriter->WriteValue(ObjDesc.Transform.GetRotation().Rotator().Roll); // Запись угла поворота Roll
	JsonWriter->WriteArrayEnd(); // Конец записи массива

	JsonWriter->WriteArrayStart("Scale"); // Начало записи массива "Scale"
	JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().X); // Запись масштаба по X
	JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().Y); // Запись масштаба по Y
	JsonWriter->WriteValue(ObjDesc.Transform.GetScale3D().Z); // Запись масштаба по Z
	JsonWriter->WriteArrayEnd(); // Конец записи массива

	if (ObjDesc.Container.Num() > 0) { // Проверка наличия контейнеров
		JsonWriter->WriteArrayStart("Containers"); // Начало записи массива "Containers"

		JsonWriter->WriteObjectStart(); // Начало записи объекта
		JsonWriter->WriteObjectStart("Container"); // Начало записи объекта "Container"

		FString Name = GetName(); // Получение имени
		JsonWriter->WriteValue("Name", TEXT("Container")); // Запись имени контейнера

		JsonWriter->WriteArrayStart("Content"); // Начало записи массива "Content"

		for (const auto& TmpStack : ObjDesc.Container) { // Проход по контейнерам
			if (TmpStack.Stack.Amount > 0) { // Проверка на наличие объектов в стеке
				JsonWriter->WriteObjectStart(); // Начало записи объекта

				const ASandboxObject* SandboxObject = Cast<ASandboxObject>(TmpStack.Stack.GetObject()); // Приведение типа к объекту песочницы
				FString ClassName = SandboxObject->GetClass()->GetName(); // Получение имени класса
				JsonWriter->WriteValue("SlotId", TmpStack.SlotId); // Запись ID слота
				JsonWriter->WriteValue("Class", ClassName); // Запись имени класса
				JsonWriter->WriteValue("ClassId", (int64)SandboxObject->GetSandboxClassId()); // Запись ID класса
				JsonWriter->WriteValue("TypeId", SandboxObject->GetSandboxTypeId()); // Запись ID типа
				JsonWriter->WriteValue("Amount", TmpStack.Stack.Amount); // Запись количества объектов

				JsonWriter->WriteObjectEnd(); // Конец записи объекта
			}
		}
		JsonWriter->WriteArrayEnd(); // Конец записи массива

		JsonWriter->WriteObjectEnd(); // Конец записи объекта
		JsonWriter->WriteObjectEnd(); // Конец записи объекта

		JsonWriter->WriteArrayEnd(); // Конец записи массива
	}

	if (ObjDesc.PropertyMap.Num() > 0) { // Проверка наличия свойств
		JsonWriter->WriteObjectStart("Properties"); // Начало записи объекта "Properties"
		for (auto& Itm : ObjDesc.PropertyMap) { // Проход по свойствам
			JsonWriter->WriteValue(Itm.Key, Itm.Value); // Запись каждого свойства
		}
		JsonWriter->WriteObjectEnd(); // Конец записи объекта
	}

	JsonWriter->WriteObjectEnd(); // Конец записи объекта
	JsonWriter->WriteObjectEnd(); // Конец записи объекта
}

void ASandboxLevelController::SavePreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) { // Метод для сохранения подготовленных объектов
	UE_LOG(LogTemp, Log, TEXT("----------- save level json -----------")); // Логирование начала сохранения уровня

	FString JsonStr; // Строка для хранения JSON
	FString FileName = TEXT("level.json"); // Имя файла для сохранения
	FString SavePath = FPaths::ProjectSavedDir(); // Путь для сохранения в проекте
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName; // Полный путь к файлу

	UE_LOG(LogTemp, Log, TEXT("level json path -> %s"), *FullPath); // Логирование полного пути

	TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr); // Создание JSON писателя

	JsonWriter->WriteObjectStart(); // Начало записи объекта

	SaveLevelJsonExt(JsonWriter); // Сохранение расширенных данных уровня

	JsonWriter->WriteArrayStart("SandboxObjectList"); // Начало записи массива объектов песочницы

	for (const auto& ObjDesc : ObjDescList) { // Проход по списку объектов
		SaveObject(JsonWriter, ObjDesc); // Сохранение каждого объекта
	}

	JsonWriter->WriteArrayEnd(); // Конец записи массива

	JsonWriter->WriteObjectEnd(); // Конец записи объекта
	JsonWriter->Close(); // Закрытие писателя

	FFileHelper::SaveStringToFile(*JsonStr, *FullPath); // Сохранение строки JSON в файл
}

FSandboxObjectDescriptor FSandboxObjectDescriptor::MakeObjDescriptor(ASandboxObject* SandboxObject) { // Метод для создания дескриптора объекта
	FSandboxObjectDescriptor ObjDesc; // Создание дескриптора объекта
	ObjDesc.ClassId = SandboxObject->GetSandboxClassId(); // Получение ID класса
	ObjDesc.TypeId = SandboxObject->GetSandboxTypeId(); // Получение ID типа
	ObjDesc.Transform = SandboxObject->GetTransform(); // Получение трансформации объекта
	ObjDesc.PropertyMap = SandboxObject->PropertyMap; // Получение карты свойств
	ObjDesc.NetUid = SandboxObject->GetSandboxNetUid(); // Получение сетевого UID

	UContainerComponent* Container = SandboxObject->GetContainer(TEXT("ObjectContainer")); // Получение компонента контейнера
	if (Container) { // Проверка наличия контейнера
		TArray<FContainerStack> Content = Container->Content; // Получение содержимого контейнера
		int SlotId = 0; // Инициализация ID слота
		for (const auto& Stack : Content) { // Проход по содержимому
			if (Stack.Amount > 0) { // Проверка наличия объектов в стеке
				if (Stack.GetObject()) { // Проверка наличия объекта
					FTempContainerStack TempContainerStack; // Создание временного стека контейнера
					TempContainerStack.Stack = Stack; // Установка стека
					TempContainerStack.SlotId = SlotId; // Установка ID слота
					ObjDesc.Container.Add(TempContainerStack); // Добавление в дескриптор
				}
			}

			SlotId++; // Увеличение ID слота
		}
	}

	return ObjDesc; // Возврат дескриптора объекта
}

void ASandboxLevelController::PrepareObjectForSave(TArray<FSandboxObjectDescriptor>& ObjDescList) { // Метод для подготовки объектов к сохранению
	for (TActorIterator<ASandboxObject> ActorItr(GetWorld()); ActorItr; ++ActorItr) { // Итерация по всем объектам песочницы
		ASandboxObject* SandboxObject = Cast<ASandboxObject>(*ActorItr); // Приведение типа к объекту песочницы
		if (SandboxObject) { // Проверка на наличие объекта
			if (SandboxObject->GetParentActor()) { // Проверка на наличие родительского актора
				continue; // Пропуск итерации
			}

			FSandboxObjectDescriptor ObjDesc = FSandboxObjectDescriptor::MakeObjDescriptor(SandboxObject); // Создание дескриптора объекта
			ObjDescList.Add(ObjDesc); // Добавление дескриптора в список
		}
	}
}

void ASandboxLevelController::SaveLevelJson() { // Метод для сохранения уровня в JSON
	TArray<FSandboxObjectDescriptor> ObjDescList; // Список дескрипторов объектов
	PrepareObjectForSave(ObjDescList); // Подготовка объектов к сохранению
	SavePreparedObjects(ObjDescList); // Сохранение подготовленных объектов
}

void ASandboxLevelController::SaveLevelJsonExt(TSharedRef <TJsonWriter<TCHAR>> JsonWriter) { // Метод для сохранения расширенных данных уровня

}

void ASandboxLevelController::PrepareMetaData() { // Метод для подготовки метаданных
	if (bIsMetaDataReady) { // Проверка на готовность метаданных
		return; // Выход из метода
	}

	if (!ObjectMap) { // Проверка на наличие карты объектов
		UE_LOG(LogTemp, Error, TEXT("ASandboxLevelController::LoadLevelJson() -----  ObjectMap == NULL")); // Логирование ошибки
		return; // Выход из метода
	}

	UE_LOG(LogTemp, Log, TEXT("----------- load object map -----------")); // Логирование начала загрузки карты объектов

	ObjectMapByClassName.Empty(); // Очистка карты объектов по имени класса
	ObjectMapById.Empty(); // Очистка карты объектов по ID

	TArray<TSubclassOf<ASandboxObject>> ObjectListCopy = ObjectMap->ObjectList; // Копирование списка объектов
	ObjectListCopy.Sort([](const TSubclassOf<ASandboxObject>& ip1, const TSubclassOf<ASandboxObject>& ip2) { // Сортировка списка объектов
		ASandboxObject* SandboxObject1 = Cast<ASandboxObject>(ip1->ClassDefaultObject); // Приведение типа к объекту песочницы
		ASandboxObject* SandboxObject2 = Cast<ASandboxObject>(ip2->ClassDefaultObject); // Приведение типа к объекту песочницы
		uint64 ClassId1 = SandboxObject1->GetSandboxClassId(); // Получение ID класса первого объекта
		uint64 ClassId2 = SandboxObject2->GetSandboxClassId(); // Получение ID класса второго объекта
		return  ClassId1 < ClassId2; // Сравнение ID классов
	});

	for (const TSubclassOf<ASandboxObject>& SandboxObjectSubclass : ObjectListCopy) { // Проход по отсортированному списку объектов
		//TSubclassOf<ASandboxObject> SandboxObjectSubclass = Elem.Value; // Закомментированная строка
		ASandboxObject* SandboxObject = Cast<ASandboxObject>(SandboxObjectSubclass->ClassDefaultObject); // Приведение типа к объекту песочницы
		uint64 ClassId = SandboxObject->GetSandboxClassId(); // Получение ID класса
		FString ClassName = SandboxObjectSubclass->ClassDefaultObject->GetClass()->GetName(); // Получение имени класса

		if (ClassId == 0) { // Проверка на наличие ID класса
			UE_LOG(LogTemp, Error, TEXT("ClassName -> %s has no class id"), *ClassName); // Логирование ошибки
			continue; // Пропуск итерации
		}

		UE_LOG(LogTemp, Log, TEXT("%s -> %d"), *ClassName, ClassId); // Логирование имени класса и ID
		ObjectMapByClassName.Add(ClassName, SandboxObjectSubclass); // Добавление в карту объектов по имени класса
		ObjectMapById.Add(ClassId, SandboxObjectSubclass); // Добавление в карту объектов по ID
	}

	bIsMetaDataReady = true; // Установка флага готовности метаданных
}

void ASandboxLevelController::LoadLevelJson() { // Метод для загрузки уровня из JSON
	if (!ObjectMap) { // Проверка на наличие карты объектов
		return; // Выход из метода
	}

	PrepareMetaData(); // Подготовка метаданных

	UE_LOG(LogTemp, Log, TEXT("----------- load level json -----------")); // Логирование начала загрузки уровня

	FString FileName = TEXT("level.json"); // Имя файла для загрузки
	FString SavePath = FPaths::ProjectSavedDir(); // Путь для загрузки из проекта
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName; // Полный путь к файлу

	FString JsonRaw; // Строка для хранения сырых данных JSON
	if (!FFileHelper::LoadFileToString(JsonRaw, *FullPath)) { // Загрузка файла в строку
		UE_LOG(LogTemp, Error, TEXT("Error loading json file")); // Логирование ошибки
	}

	TArray<FSandboxObjectDescriptor> ObjDescList; // Список дескрипторов объектов

	TSharedPtr<FJsonObject> JsonParsed; // Указатель на разобранный JSON объект
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonRaw); // Создание читателя JSON
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) { // Десериализация JSON
		TArray <TSharedPtr<FJsonValue>> SandboxObjectList = JsonParsed->GetArrayField("SandboxObjectList"); // Получение массива объектов песочницы
		for (int Idx = 0; Idx < SandboxObjectList.Num(); Idx++) { // Проход по массиву объектов
			TSharedPtr<FJsonObject> ObjPtr = SandboxObjectList[Idx]->AsObject(); // Приведение значения к JSON объекту
			TSharedPtr<FJsonObject> SandboxObjectPtr = ObjPtr->GetObjectField(TEXT("Object")); // Получение объекта "Object"

			FSandboxObjectDescriptor ObjDesc; // Создание дескриптора объекта

			ObjDesc.ClassId = SandboxObjectPtr->GetIntegerField(TEXT("ClassId")); // Получение ID класса
			ObjDesc.TypeId = SandboxObjectPtr->GetIntegerField(TEXT("TypeId")); // Получение ID типа
			ObjDesc.NetUid = SandboxObjectPtr->GetStringField(TEXT("NetUid")); // Получение сетевого UID

			FVector Location; // Вектор для хранения местоположения
			TArray <TSharedPtr<FJsonValue>> LocationValArray = SandboxObjectPtr->GetArrayField("Location"); // Получение массива местоположений
			Location.X = LocationValArray[0]->AsNumber(); // Запись координаты X
			Location.Y = LocationValArray[1]->AsNumber(); // Запись координаты Y
			Location.Z = LocationValArray[2]->AsNumber(); // Запись координаты Z

			FRotator Rotation; // Ротор для хранения углов поворота
			TArray <TSharedPtr<FJsonValue>>  RotationValArray = SandboxObjectPtr->GetArrayField("Rotation"); // Получение массива углов поворота
			Rotation.Pitch = RotationValArray[0]->AsNumber(); // Запись угла поворота Pitch
			Rotation.Yaw = RotationValArray[1]->AsNumber(); // Запись угла поворота Yaw
			Rotation.Roll = RotationValArray[2]->AsNumber(); // Запись угла поворота Roll

			FVector Scale; // Вектор для хранения масштаба
			TArray <TSharedPtr<FJsonValue>> ScaleValArray = SandboxObjectPtr->GetArrayField("Scale"); // Получение массива масштаба
			Scale.X = ScaleValArray[0]->AsNumber(); // Запись масштаба по X
			Scale.Y = ScaleValArray[1]->AsNumber(); // Запись масштаба по Y
			Scale.Z = ScaleValArray[2]->AsNumber(); // Запись масштаба по Z

			ObjDesc.Transform = FTransform(Rotation, Location, Scale); // Установка трансформации объекта

			const TSharedPtr<FJsonObject>* ObjPropJson = nullptr; // Указатель для свойств объекта
			bool bHasProperties = SandboxObjectPtr->TryGetObjectField("Properties", ObjPropJson); // Проверка наличия свойств
			if (bHasProperties) { // Если свойства существуют
				for (auto& Itm : (*ObjPropJson)->Values) { // Проход по свойствам
					FString Key = Itm.Key; // Получение ключа свойства
					FString Value = Itm.Value->AsString(); // Получение значения свойства
					ObjDesc.PropertyMap.Add(Key, Value); // Добавление свойства в дескриптор
				}
			}

			const TArray<TSharedPtr<FJsonValue>>* ContainersJsonArray = nullptr; // Указатель для массива контейнеров
			bool bHasContainers = SandboxObjectPtr->TryGetArrayField("Containers", ContainersJsonArray); // Проверка наличия контейнеров
			if (bHasContainers) { // Если контейнеры существуют
				for (TSharedPtr<FJsonValue> ContainerJsonValue : *ContainersJsonArray) { // Проход по массиву контейнеров
					TSharedPtr<FJsonObject> ContainerJsonObject = ContainerJsonValue->AsObject()->GetObjectField(TEXT("Container")); // Получение контейнера
					FString ContainerName = ContainerJsonObject->GetStringField("Name"); // Получение имени контейнера
					TArray<TSharedPtr<FJsonValue>> ContentArray = ContainerJsonObject->GetArrayField("Content"); // Получение массива содержимого контейнера
					//UE_LOG(LogTemp, Warning, TEXT("ContainerName %s"), *ContainerName); // Логирование имени контейнера (закомментировано)

					for (TSharedPtr<FJsonValue> ContentJsonValue : ContentArray) { // Проход по массиву содержимого
						int SlotId = ContentJsonValue->AsObject()->GetIntegerField("SlotId"); // Получение ID слота
						int ClassId = ContentJsonValue->AsObject()->GetIntegerField("ClassId"); // Получение ID класса
						int Amount = ContentJsonValue->AsObject()->GetIntegerField("Amount"); // Получение количества

						FContainerStack Stack; // Создание стека контейнера
						Stack.Amount = Amount; // Установка количества
						Stack.SandboxClassId = ClassId; // Установка ID класса

						FTempContainerStack TempContainerStack; // Создание временного стека контейнера
						TempContainerStack.Stack = Stack; // Установка стека
						TempContainerStack.SlotId = SlotId; // Установка ID слота

						ObjDesc.Container.Add(TempContainerStack); // Добавление временного стека в дескриптор
					}
				}
			}
			ObjDescList.Add(ObjDesc); // Добавление дескриптора в список
		}

		LoadLevelJsonExt(JsonParsed); // Загрузка расширенных данных уровня
	}

	SpawnPreparedObjects(ObjDescList); // Спавн подготовленных объектов
}

ASandboxObject* ASandboxLevelController::SpawnPreparedObject(const FSandboxObjectDescriptor& ObjDesc) { // Метод для спавна подготовленного объекта
	ASandboxObject* NewObject = SpawnSandboxObject(ObjDesc.ClassId, ObjDesc.Transform, ObjDesc.NetUid); // Спавн нового объекта
	if (NewObject) { // Проверка на успешный спавн
		NewObject->PropertyMap = ObjDesc.PropertyMap; // Установка карты свойств
		NewObject->PostLoadProperties(); // Вызов метода для загрузки свойств

		UContainerComponent* Container = NewObject->GetContainer(TEXT("ObjectContainer")); // Получение компонента контейнера
		if (Container) { // Проверка наличия контейнера
			for (const auto& Itm : ObjDesc.Container) { // Проход по контейнерам
				Container->SetStackDirectly(Itm.Stack, Itm.SlotId); // Установка стека контейнера
			}
		}
	}

	return NewObject; // Возврат нового объекта
}

void ASandboxLevelController::SpawnPreparedObjects(const TArray<FSandboxObjectDescriptor>& ObjDescList) { // Метод для спавна подготовленных объектов
	for (const auto& ObjDesc : ObjDescList) { // Проход по списку дескрипторов объектов
		SpawnPreparedObject(ObjDesc); // Спавн каждого подготовленного объекта
	}
}

FString ASandboxLevelController::GetNewUid() const { // Метод для получения нового UID

	std::random_device rd; // Генератор случайных чисел
	std::mt19937_64 gen(rd()); // Инициализация генератора
	std::uniform_int_distribution<uint64_t> dis; // Распределение случайных чисел

	FString UID; // Строка для хранения UID

	do {
		uint64 uid1 = dis(gen); // Генерация первого UID
		uint64 uid2 = dis(gen); // Генерация второго UID
		uint64 uid3 = dis(gen); // Генерация третьего UID

		UID = FString::Printf(TEXT("%llx-%llx-%llx"), uid1, uid2, uid3); // Форматирование строки UID

	} while (GlobalObjectMap.Find(UID) != nullptr); // Проверка на уникальность UID

	return UID; // Возврат уникального UID
}

ASandboxObject* ASandboxLevelController::SpawnSandboxObject(const int ClassId, const FTransform& Transform, const FString& SandboxNetUid) { // Метод для спавна объекта песочницы
	if (GetNetMode() != NM_Client) { // Проверка, что не в клиентском режиме
		TSubclassOf<ASandboxObject> SandboxObject = GetSandboxObjectByClassId(ClassId); // Получение объекта по ID класса
		if (SandboxObject) { // Проверка, что объект существует
			UClass* SpawnClass = SandboxObject->ClassDefaultObject->GetClass(); // Получение класса по умолчанию
			ASandboxObject* NewObject = (ASandboxObject*)GetWorld()->SpawnActor(SpawnClass, &Transform); // Спавн нового объекта
			if (NewObject) { // Проверка на успешный спавн
				NewObject->SandboxNetUid = (SandboxNetUid == "") ? GetNewUid() : SandboxNetUid; // Установка сетевого UID
				GlobalObjectMap.Add(NewObject->SandboxNetUid, NewObject); // Добавление объекта в глобальную карту
			}
			return NewObject; // Возврат нового объекта
		}
	}

	return nullptr; // Возврат nullptr, если объект не был создан
}

void ASandboxLevelController::LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed) { // Метод для загрузки расширенных данных уровня

}

TSubclassOf<ASandboxObject> ASandboxLevelController::GetSandboxObjectByClassId(int32 ClassId) { // Метод для получения объекта песочницы по ID класса
	if (!bIsMetaDataReady) { // Проверка на готовность метаданных
		PrepareMetaData(); // Подготовка метаданных
	}

	if (ObjectMapById.Contains(ClassId)) { // Проверка на наличие объекта в карте по ID
		return ObjectMapById[ClassId]; // Возврат объекта
	}

	return nullptr; // Возврат nullptr, если объект не найден
}

ASandboxObject* ASandboxLevelController::GetDefaultSandboxObject(uint64 ClassId) { // Метод для получения объекта песочницы по умолчанию по ID класса
	if (ObjectMapById.Contains(ClassId)) { // Проверка на наличие объекта в карте по ID
		return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject()); // Возврат объекта по умолчанию
	}

	return nullptr; // Возврат nullptr, если объект не найден
}

ASandboxLevelController* ASandboxLevelController::GetInstance() { // Метод для получения экземпляра контроллера уровня
	return StaticSelf; // Возврат статической ссылки на экземпляр
}

ASandboxObject* ASandboxLevelController::GetSandboxObject(uint64 ClassId) { // Метод для получения объекта песочницы по ID класса
	if (!bIsMetaDataReady) { // Проверка на готовность метаданных
		PrepareMetaData(); // Подготовка метаданных
	}

	if (ObjectMapById.Contains(ClassId)) { // Проверка на наличие объекта в карте по ID
		return (ASandboxObject*)(ObjectMapById[ClassId]->GetDefaultObject()); // Возврат объекта по умолчанию
	}

	return nullptr; // Возврат nullptr, если объект не найден
}

bool ASandboxLevelController::RemoveSandboxObject(ASandboxObject* Obj) { // Метод для удаления объекта песочницы
	if (GetNetMode() != NM_Client) { // Проверка, что не в клиентском режиме
		if (Obj) { // Проверка на наличие объекта
			FString NetUid = Obj->GetSandboxNetUid(); // Получение сетевого UID
			Obj->Destroy(); // Уничтожение объекта
			GlobalObjectMap.Remove(NetUid); // Удаление объекта из глобальной карты
			return true; // Возврат true, если объект был удален
		}
	}

	return false; // Возврат false, если объект не был удален
}

ASandboxObject* ASandboxLevelController::GetObjectByNetUid(FString NetUid) { // Метод для получения объекта по сетевому UID
	if (GlobalObjectMap.Contains(NetUid)) { // Проверка на наличие объекта в глобальной карте
		return GlobalObjectMap[NetUid]; // Возврат объекта
	}

	return nullptr; // Возврат nullptr, если объект не найден
}

ASandboxEffect* ASandboxLevelController::SpawnEffect(const int32 EffectId, const FTransform& Transform) { // Метод для спавна эффекта
	if (GetNetMode() != NM_Client) { // Проверка, что не в клиентском режиме
		if (ObjectMap->Effects.Contains(EffectId)) { // Проверка на наличие эффекта по ID
			TSubclassOf<ASandboxEffect> Effect = ObjectMap->Effects[EffectId]; // Получение эффекта
			if (Effect) { // Проверка, что эффект существует
				UClass* SpawnClass = Effect->ClassDefaultObject->GetClass(); // Получение класса по умолчанию
				return (ASandboxEffect*)GetWorld()->SpawnActor(SpawnClass, &Transform); // Спавн эффекта
			}
		}
	}

	return nullptr; // Возврат nullptr, если эффект не был создан
}
