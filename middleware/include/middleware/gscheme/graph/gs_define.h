#pragma once

// based on cpgf/gmetadefine.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#endif

#include "cpgf/gmetaannotation.h"
#include "cpgf/gmetaclass.h"
#include "cpgf/gmetaenum.h"
#include "cpgf/gmetafield.h"
#include "cpgf/gmetamethod.h"
#include "cpgf/gmetaproperty.h"
#include "cpgf/gglobal.h"
#include "cpgf/gpp.h"

#pragma GCC diagnostic pop

#include <memory>

// #define MAX_BASE_COUNT 20
#define MAX_BASE_COUNT 1
#define BASE_DEFAULT(N, unused) , typename GPP_CONCAT(BaseType, N) = void

namespace cpgf {

namespace meta_internal {


#define CASE_SUPERLIST_ARG(N, unused) case N: superList->add<ClassType, typename TypeList_GetWithDefault<TList, N, void>::Result>(); break;

template <typename TList, typename ClassType>
GMetaSuperList * doMakeSuperList() {
	GMetaSuperList * superList = new GMetaSuperList;

	for(int i = 0; i < MAX_BASE_COUNT; ++i) {
		if(i >= static_cast<int>(TypeList_Length<TList>::Result)) {
			break;
		}

		switch(i) {
			GPP_REPEAT(19, CASE_SUPERLIST_ARG, GPP_EMPTY())

			default:
				break;
		}
	}

	return superList;
}

#undef CASE_SUPERLIST_ARG

typedef std::shared_ptr<GMetaClass> GSharedMetaClass;

struct GSharedMetaClassDeleter
{
	explicit GSharedMetaClassDeleter(const bool freeIt = true)
		: freeIt(freeIt)
	{
	}

	void operator() (GMetaClass * p) const {
		if(freeIt) {
			delete p;
		}
	}

	bool freeIt;
};

inline GSharedMetaClass makeSharedMetaClass(GMetaClass * metaClass, const bool autoFreeIt = true)
{
	return GSharedMetaClass(metaClass, GSharedMetaClassDeleter(autoFreeIt));
}

inline GMetaClass * takeSharedMetaClass(GSharedMetaClass * metaClass)
{
	std::get_deleter<GSharedMetaClassDeleter>(*metaClass)->freeIt = false;
	return metaClass->get();
}


} // namespace meta_internal


template <typename BaseType>
class GDefineMetaConstructor : public BaseType
{
private:
	typedef GDefineMetaConstructor<BaseType> ThisType;

public:
	GDefineMetaConstructor(meta_internal::GSharedMetaClass metaClass, GMetaConstructor * constructor) : BaseType(metaClass, constructor) {
	}

	ThisType _default(const GVariant & value) {
		gdynamic_cast<GMetaConstructor *>(this->currentItem)->addDefaultParam(value);

		return *this;
	}
};

template <typename BaseType>
class GDefineMetaMethod : public BaseType
{
private:
	typedef GDefineMetaMethod<BaseType> ThisType;

public:
	GDefineMetaMethod(meta_internal::GSharedMetaClass metaClass, GMetaMethod * method) : BaseType(metaClass, method) {
	}

	ThisType _default(const GVariant & value) {
		gdynamic_cast<GMetaMethod *>(this->currentItem)->addDefaultParam(value);

		return *this;
	}
};

template <typename BaseType>
class GDefineMetaEnum : public BaseType
{
private:
	typedef GDefineMetaEnum<BaseType> ThisType;

public:
	GDefineMetaEnum(meta_internal::GSharedMetaClass metaClass, GMetaEnum * metaEnum) : BaseType(metaClass, metaEnum) {
	}

	ThisType _element(const char * key, const GVariant & value) {
		(*gdynamic_cast<GMetaEnum *>(this->currentItem))(key, value);

		return *this;
	}

};

template <typename BaseType>
class GDefineMetaAnnotation : public BaseType
{
public:
	GDefineMetaAnnotation(meta_internal::GSharedMetaClass metaClass, GMetaAnnotation * annotation) : BaseType(metaClass, annotation) {
	}

	template <typename T>
	GDefineMetaAnnotation _element(const char * name, const T & value) {
		(*gdynamic_cast<GMetaAnnotation *>(this->currentItem))(name, value);

		return *this;
	}

};

template <typename BaseType>
class GDefineMetaField : public BaseType
{
public:
	GDefineMetaField(meta_internal::GSharedMetaClass metaClass, GMetaField * field) : BaseType(metaClass, field) {
	}

};

template <typename BaseType>
class GDefineMetaProperty : public BaseType
{
public:
	GDefineMetaProperty(meta_internal::GSharedMetaClass metaClass, GMetaProperty * prop) : BaseType(metaClass, prop) {
	}

};

template <typename ClassType GPP_REPEAT(MAX_BASE_COUNT, BASE_DEFAULT, GPP_EMPTY)>
class GDefineMetaClass;

template <typename ClassT, typename DerivedType>
class GDefineMetaCommon
{
public:
	typedef ClassT ClassType;

private:
	typedef GDefineMetaCommon<ClassType, DerivedType> ThisType;

public:
	GDefineMetaCommon(meta_internal::GSharedMetaClass metaClass, GMetaItem * currentItem)
		: metaClass(metaClass), dangling(false), currentItem(currentItem) {
	}

	GDefineMetaCommon(const GDefineMetaCommon & other)
		: metaClass(other.metaClass), dangling(other.dangling), currentItem(other.currentItem) {
	}

	GDefineMetaCommon operator = (const GDefineMetaCommon & other) {
		this->metaClass = other.metaClass;
		this->dangling = other.dangling;
		this->currentItem = other.currentItem;

		return *this;
	}

	template <typename FT>
	GDefineMetaMethod<DerivedType> _method(const char * name, FT func) {
		return GDefineMetaMethod<DerivedType>(
			this->metaClass,
			this->metaClass->addMethod(GMetaMethod::newMethod<ClassType>(name, func, GMetaPolicyDefault()))
		);
	}

	template <typename FT, typename Policy>
	GDefineMetaMethod<DerivedType> _method(const char * name, FT func, const Policy & policy) {
		return GDefineMetaMethod<DerivedType>(
			this->metaClass,
			this->metaClass->addMethod(GMetaMethod::newMethod<ClassType>(name, func, policy))
		);
	}

	template <typename FT>
	GDefineMetaField<DerivedType> _field(const char * name, FT field) {
		return GDefineMetaField<DerivedType>(
			this->metaClass,
			this->metaClass->addField(new GMetaField(name, field, GMetaPolicyDefault()))
		);
	}

	template <typename FT, typename Policy>
	GDefineMetaField<DerivedType> _field(const char * name, FT field, const Policy & policy) {
		return GDefineMetaField<DerivedType>(
			this->metaClass,
			this->metaClass->addField(new GMetaField(name, field, policy))
		);
	}

	template <typename Getter, typename Setter>
	GDefineMetaProperty<DerivedType> _property(const char * name, const Getter & getter, const Setter & setter) {
		return GDefineMetaProperty<DerivedType>(
			this->metaClass,
			this->metaClass->addProperty(new GMetaProperty(name, getter, setter, GMetaPolicyDefault()))
		);
	}

	template <typename Getter, typename Setter, typename Policy>
	GDefineMetaProperty<DerivedType> _property(const char * name, const Getter & getter, const Setter & setter, const Policy & policy) {
		return GDefineMetaProperty<DerivedType>(
			this->metaClass,
			this->metaClass->addProperty(new GMetaProperty(name, getter, setter, policy))
		);
	}

	template <typename T>
	GDefineMetaEnum<DerivedType> _enum(const char * name) {
		return GDefineMetaEnum<DerivedType>(
			this->metaClass,
			this->metaClass->addEnum(new GMetaEnum(name, createMetaType<T>()))
		);
	}

	GDefineMetaAnnotation<DerivedType> _annotation(const char * name) {
		return GDefineMetaAnnotation<DerivedType>(
			this->metaClass,
			this->currentItem->addItemAnnotation(new GMetaAnnotation(name))
		);
	}

protected:
	meta_internal::GSharedMetaClass metaClass;
	bool dangling;
	GMetaItem * currentItem;
};

template <typename ClassType GPP_REPEAT_TAIL_PARAMS(MAX_BASE_COUNT, typename BaseType)>
class GDefineMetaClass : public GDefineMetaCommon<ClassType, GDefineMetaClass<ClassType GPP_REPEAT_TAIL_PARAMS(MAX_BASE_COUNT, BaseType)> >
{
private:
	typedef GDefineMetaClass<ClassType GPP_REPEAT_TAIL_PARAMS(MAX_BASE_COUNT, BaseType)> ThisType;
	typedef GDefineMetaCommon<ClassType, ThisType > super;

public:
	static ThisType define(const char * className) {
		ThisType c;
		c.init(className);
		return c;
	}

protected:
	GDefineMetaClass() : super(meta_internal::GSharedMetaClass(), nullptr) {
	}

	explicit GDefineMetaClass(GMetaClass * metaClass) : super(meta_internal::makeSharedMetaClass(metaClass), metaClass) {
		this->takeMetaClass();
	}

	GDefineMetaClass(GMetaClass * metaClass, GMetaItem * currentItem) : super(meta_internal::makeSharedMetaClass(metaClass), currentItem) {
		this->takeMetaClass();
	}

	GDefineMetaClass(meta_internal::GSharedMetaClass metaClass, GMetaItem * currentItem) : super(metaClass, currentItem) {
	}

public:
	template <typename FT>
	GDefineMetaConstructor<ThisType> _constructor() {
		return GDefineMetaConstructor<ThisType>(
			this->metaClass,
			this->metaClass->addConstructor(GMetaConstructor::newConstructor<ClassType, FT>(GMetaPolicyDefault()))
		);
	}

	template <typename FT, typename Policy>
	GDefineMetaConstructor<ThisType> _constructor(const Policy & policy) {
		return GDefineMetaConstructor<ThisType>(
			this->metaClass,
			this->metaClass->addConstructor(GMetaConstructor::newConstructor<ClassType, FT>(policy))
		);
	}

	template <typename FT>
	GDefineMetaConstructor<ThisType> _constructor(const FT & func) {
		return GDefineMetaConstructor<ThisType>(
			this->metaClass,
			this->metaClass->addConstructor(GMetaConstructor::newConstructor<ClassType, FT>(func, GMetaPolicyDefault()))
		);
	}

	template <typename FT, typename Policy>
	GDefineMetaConstructor<ThisType> _constructor(const FT & func, const Policy & policy) {
		return GDefineMetaConstructor<ThisType>(
			this->metaClass,
			this->metaClass->addConstructor(GMetaConstructor::newConstructor<ClassType, FT>(func, policy))
		);
	}

protected:
	typedef typename cpgf::TypeList_Make<GPP_REPEAT_PARAMS(MAX_BASE_COUNT, BaseType)>::Result BaseListType;

	void init(const char * className) {
		GMetaClass* classToAdd = const_cast<GMetaClass *>(getGlobalMetaClass()->doGetClass(className));

		if(classToAdd == nullptr) {
            ClassType* classType = nullptr;
            const auto policy = GMetaPolicyDefault();
			classToAdd = new GMetaClass(classType, meta_internal::doMakeSuperList<BaseListType, ClassType>(), className, nullptr, policy);
			getGlobalMetaClass()->addClass(classToAdd);
		}

		this->metaClass = meta_internal::makeSharedMetaClass(classToAdd);
		meta_internal::takeSharedMetaClass(&this->metaClass);

		this->currentItem = classToAdd;
	}
};

} // namespace cpgf

#undef MAX_BASE_COUNT
#undef BASE_DEFAULT
