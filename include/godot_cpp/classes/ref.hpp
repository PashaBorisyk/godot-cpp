/**************************************************************************/
/*  ref.hpp                                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <godot_cpp/core/defs.hpp>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

// Helper class for RefCounted objects, same as Godot one.

/**
 * Owns a strong reference to a `godot::RefCounted` instance. Releasing it can destroy the instance.
 *
 * **Used for** keeping resources alive while an extension stores or passes them.
 *
 * - `T` — Engine wrapper type derived from `godot::RefCounted`.
 */
template <typename T>
class Ref {
	/**
	 * Points to the held C++ instance, or is null when empty.
	 */
	T *reference = nullptr;

	/**
	 * Acquires the reference held by another wrapper and releases the previous one.
	 */
	_FORCE_INLINE_ void ref(const Ref &p_from) {
		ref_pointer<false>(p_from.reference);
	}

	/**
	 * Replaces the held pointer and acquires an engine reference, clearing the pointer if acquisition fails.
	 *
	 * - `Init` — Uses `godot::RefCounted::init_ref` for acquisition when true and `godot::RefCounted::reference` otherwise.
	 */
	template <bool Init>
	_FORCE_INLINE_ void ref_pointer(T *p_refcounted) {
		if (p_refcounted == reference) {
			return;
		}

		// This will go out of scope and get unref'd.
		Ref cleanup_ref;
		cleanup_ref.reference = reference;
		reference = p_refcounted;
		if (reference) {
			if constexpr (Init) {
				if (!reference->init_ref()) {
					reference = nullptr;
				}
			} else {
				if (!reference->reference()) {
					reference = nullptr;
				}
			}
		}
	}

public:
	/**
	 * Returns the registered class name of `T`.
	 *
	 * **Used for** looking up the class in the engine when only the reference type is known.
	 */
	static _FORCE_INLINE_ String get_class_static() {
		return T::get_class_static();
	}

	/**
	 * Tests whether the stored address equals the supplied address.
	 *
	 * **Used for** matching a cached resource against a pointer received from the engine.
	 *
	 * - `p_ptr` — Address to compare. Null matches an empty reference.
	 */
	_FORCE_INLINE_ bool operator==(const T *p_ptr) const {
		return reference == p_ptr;
	}
	/**
	 * Tests whether the stored address differs from the supplied address.
	 *
	 * **Used for** detecting an engine-side resource change before updating a cache.
	 *
	 * - `p_ptr` — Address to compare. Null tests for a held instance.
	 */
	_FORCE_INLINE_ bool operator!=(const T *p_ptr) const {
		return reference != p_ptr;
	}

	/**
	 * Compares the raw pointers with the built-in `operator<`.
	 *
	 * **Used for** ordering held resource addresses in process-local diagnostics.
	 *
	 * - `p_r` — Reference whose stored pointer is compared.
	 */
	_FORCE_INLINE_ bool operator<(const Ref<T> &p_r) const {
		return reference < p_r.reference;
	}
	/**
	 * Tests whether both references hold the same pointer.
	 *
	 * **Used for** checking whether two resource slots share an instance.
	 *
	 * - `p_r` — Other reference. Two empty references compare equal.
	 */
	_FORCE_INLINE_ bool operator==(const Ref<T> &p_r) const {
		return reference == p_r.reference;
	}
	/**
	 * Tests whether the references hold different pointers.
	 *
	 * **Used for** refreshing a preview when its resource instance changes.
	 *
	 * - `p_r` — Other reference. An empty one holds a null pointer.
	 */
	_FORCE_INLINE_ bool operator!=(const Ref<T> &p_r) const {
		return reference != p_r.reference;
	}

	/**
	 * Returns the held pointer, or `nullptr`.
	 *
	 * **Used for** passing a resource to a raw-pointer API while this reference holds it.
	 */
	_FORCE_INLINE_ T *operator*() const {
		return reference;
	}

	/**
	 * Returns the held pointer, or `nullptr`.
	 * Check validity before member access.
	 *
	 * **Used for** calling a method on a resource held by the reference.
	 */
	_FORCE_INLINE_ T *operator->() const {
		return reference;
	}

	/**
	 * Returns the held pointer, or `nullptr`.
	 * The caller gains no reference.
	 *
	 * **Used for** passing a borrowed resource pointer to a native function.
	 */
	_FORCE_INLINE_ T *ptr() const {
		return reference;
	}

	/**
	 * Creates a `godot::Variant` from the held object pointer.
	 *
	 * **Used for** passing a reference-counted resource to a Variant-based call.
	 */
	operator Variant() const {
		return Variant(reference);
	}

	/**
	 * Shares the source instance and releases the old one. Equal pointers leave both references unchanged.
	 *
	 * **Used for** sharing a resource between two long-lived owners.
	 *
	 * - `p_from` — Source reference. An empty source clears the destination.
	 */
	void operator=(const Ref &p_from) {
		ref(p_from);
	}

	/**
	 * Releases the old reference, takes the source reference, and clears the source. Equal pointers leave both unchanged.
	 *
	 * **Used for** handing ownership of a resource to another wrapper.
	 *
	 * - `p_from` — Source reference. It is cleared only if the pointers differ.
	 */
	void operator=(Ref &&p_from) {
		if (reference == p_from.reference) {
			return;
		}
		unref();
		reference = p_from.reference;
		p_from.reference = nullptr;
	}

	/**
	 * Casts the source to `T` and acquires it in place of the old instance. An unsuccessful cast or acquisition clears this reference.
	 *
	 * **Used for** updating a typed resource slot from a reference to a related class.
	 *
	 * - `T_Other` — Class held by the source reference.
	 * - `p_from` — Source reference. An empty source clears the destination.
	 */
	template <typename T_Other>
	void operator=(const Ref<T_Other> &p_from) {
		ref_pointer<false>(Object::cast_to<T>(p_from.ptr()));
	}

	/**
	 * Replaces the old reference using `godot::RefCounted::init_ref()`. Equal pointers leave it unchanged. A failed acquisition clears it.
	 *
	 * **Used for** owning a raw resource returned by a native factory.
	 *
	 * - `p_from` — Live instance to acquire. Null clears the reference.
	 */
	void operator=(T *p_from) {
		ref_pointer<true>(p_from);
	}

	/**
	 * Replaces the old reference with the live object in the variant, cast to `T`. An invalid object, cast, or acquisition clears it.
	 *
	 * **Used for** updating a typed resource slot from a Variant-backed property.
	 *
	 * - `p_variant` — Variant whose live object is cast and acquired.
	 */
	void operator=(const Variant &p_variant) {
		Object *object = p_variant.get_validated_object();

		if (object == reference) {
			return;
		}

		ref_pointer<false>(Object::cast_to<T>(object));
	}

	/**
	 * Replaces the reference with a raw object cast to `T` and acquired through `godot::RefCounted::init_ref()`. A failed cast or acquisition clears it.
	 *
	 * **Used for** holding a resource after a runtime class check.
	 *
	 * - `T_Other` — Static type of the source pointer.
	 * - `p_ptr` — Live engine object to cast and acquire, or null to clear.
	 */
	template <typename T_Other>
	void reference_ptr(T_Other *p_ptr) {
		if (reference == p_ptr) {
			return;
		}

		ref_pointer<true>(Object::cast_to<T>(p_ptr));
	}

	/**
	 * Acquires a separate reference to the source instance.
	 *
	 * **Used for** keeping a resource alive in a second owner.
	 *
	 * - `p_from` — Reference to copy. The source keeps its ownership.
	 */
	Ref(const Ref &p_from) {
		this->operator=(p_from);
	}

	/**
	 * Takes the held reference and clears the source.
	 *
	 * **Used for** moving a resource into storage without changing its reference count.
	 *
	 * - `p_from` — Reference to take. It is empty after construction.
	 */
	Ref(Ref &&p_from) {
		reference = p_from.reference;
		p_from.reference = nullptr;
	}

	/**
	 * Holds the source after a successful cast and acquisition. It is empty on failure.
	 *
	 * **Used for** getting a typed reference from a resource loaded through a base class.
	 *
	 * - `T_Other` — Class held by the source reference.
	 * - `p_from` — Potentially empty source reference. Its ownership is retained.
	 */
	template <typename T_Other>
	Ref(const Ref<T_Other> &p_from) {
		this->operator=(p_from);
	}

	/**
	 * Acquires the raw instance through `godot::RefCounted::init_ref()`. It is empty if acquisition fails.
	 *
	 * **Used for** keeping a newly created raw resource alive.
	 *
	 * - `p_from` — Live instance to acquire, or null for an empty reference.
	 */
	Ref(T *p_from) {
		this->operator=(p_from);
	}

	/**
	 * Acquires the live object in the variant as `T`. It is empty if validation, casting, or acquisition fails.
	 *
	 * **Used for** reading a typed resource from a value returned by script.
	 *
	 * - `p_from` — Variant whose live object is cast and acquired.
	 */
	Ref(const Variant &p_from) {
		this->operator=(p_from);
	}

	/**
	 * Returns `true` if an instance is held.
	 *
	 * **Used for** checking that a resource was acquired before calling its methods.
	 */
	inline bool is_valid() const { return reference != nullptr; }
	/**
	 * Returns `true` if this reference is empty.
	 *
	 * **Used for** handling an optional resource that has not been assigned.
	 */
	inline bool is_null() const { return reference == nullptr; }

	/**
	 * Releases the reference and clears the pointer. Deletes the instance if the engine reports it can be destroyed.
	 *
	 * **Used for** closing a resource early while keeping the wrapper available for reuse.
	 */
	void unref() {
		if (reference) {
			// NOTE: `reinterpret_cast` is "safe" here because we know `T` has simple linear
			// inheritance to `RefCounted`. This guarantees that `T * == `RefCounted *`, which
			// allows us to declare `Ref<T>` with forward declared `T` types.
			if (reinterpret_cast<RefCounted *>(reference)->unreference()) {
				memdelete(reinterpret_cast<RefCounted *>(reference));
			}
			reference = nullptr;
		}
	}

	/**
	 * Constructs an instance of `T`, then replaces the held reference with it.
	 *
	 * **Used for** creating a resource in a reference-counted member.
	 *
	 * - `VarArgs` — Types of the arguments supplied to the constructor.
	 * - `p_params` — Arguments passed to the constructor of `T`.
	 */
	template <typename... VarArgs>
	void instantiate(VarArgs... p_params) {
		Ref<T> ref = memnew(T(p_params...));
		SWAP(reference, ref.reference);
	}

	/**
	 * Returns the hash of the held pointer.
	 *
	 * **Used for** indexing a resource by instance identity in a hash table.
	 */
	uint32_t hash() const { return HashMapHasherDefault::hash(reference); }

	/**
	 * Creates an empty reference.
	 *
	 * **Used for** declaring an optional resource before it is loaded.
	 */
	Ref() = default;

	/**
	 * Releases the held reference. The instance is deleted if the engine reports it can be destroyed.
	 *
	 * **Used for** automatically releasing resource ownership on scope exit.
	 */
	~Ref() {
		unref();
	}

	// Used exclusively in the bindings to recreate the Ref Godot encapsulates in return values,
	// without adding to the refcount.
	/**
	 * Wraps an engine instance whose reference has already been acquired for this return value.
	 */
	inline static Ref<T> _gde_internal_constructor(Object *obj) {
		Ref<T> r;
		r.reference = reinterpret_cast<T *>(obj);
		return r;
	}
};

/**
 * Selects the result of constructing a reference-counted extension class through `godot::memnew`.
 */
template <typename T>
struct memnew_result<T, std::enable_if_t<std::is_base_of_v<RefCounted, T>>> {
#if GODOT_VERSION_MINOR >= 7
	/**
	 * The owning reference wrapper returned on Godot 4.7 and later.
	 */
	using class_name = Ref<T>;
	/**
	 * Captures the engine-acquired reference without incrementing the count again.
	 */
	_ALWAYS_INLINE_ static class_name capture(T *p_obj) {
		// Godot will have already incremented the refcount, so we can create the Ref<T> without incrementing it again.
		return Ref<T>::_gde_internal_constructor(p_obj);
	}
#else
	/**
	 * The raw pointer returned for bindings before Godot 4.7.
	 */
	using class_name = T *;
	/**
	 * Returns the raw pointer unchanged for bindings before Godot 4.7.
	 */
	_ALWAYS_INLINE_ static class_name capture(class_name p_obj) { return p_obj; }
#endif
};

/**
 * Runs the object post-initialization handler on the instance held by the wrapper.
 */
template <typename T>
void postinitialize_handler(Ref<T> &p_object) {
	postinitialize_handler(p_object.ptr());
}

/**
 * Adapts an owning object reference to the GDExtension pointer-call argument convention.
 */
template <typename T>
struct PtrToArg<Ref<T>> {
	/**
	 * Reads an engine reference slot and acquires a C++ reference to the bound instance. A null slot pointer yields an empty wrapper.
	 */
	_FORCE_INLINE_ static Ref<T> convert(const void *p_ptr) {
		GDExtensionRefPtr ref = (GDExtensionRefPtr)p_ptr;
		if (unlikely(!p_ptr)) {
			return Ref<T>();
		}
		return Ref<T>(reinterpret_cast<T *>(::godot::internal::get_object_instance_binding(::godot::gdextension_interface::ref_get_object(ref))));
	}

	/**
	 * The owning reference type accepted by the return-slot encoder.
	 */
	typedef Ref<T> EncodeT;

	/**
	 * Sets an uninitialized engine reference slot to the instance held by the wrapper when it has one.
	 */
	_FORCE_INLINE_ static void encode(Ref<T> p_val, void *p_ptr) {
		GDExtensionRefPtr ref = (GDExtensionRefPtr)p_ptr;
		ERR_FAIL_NULL(ref);

		// This code assumes that p_ptr points to an unset Ref<T> variable on the Godot side
		// so we only set it if we have an object to set.
		if (p_val.is_valid()) {
			::godot::gdextension_interface::ref_set_object(ref, p_val->_owner);
		}
	}
};

/**
 * Adapts a const reference parameter to a temporary owning reference for a pointer call.
 */
template <typename T>
struct PtrToArg<const Ref<T> &> {
	/**
	 * The owning wrapper type used for pointer-call argument storage.
	 */
	typedef Ref<T> EncodeT;

	/**
	 * Reads the engine reference slot and returns an owning C++ wrapper. A null slot pointer yields an empty wrapper.
	 */
	_FORCE_INLINE_ static Ref<T> convert(const void *p_ptr) {
		GDExtensionRefPtr ref = const_cast<GDExtensionRefPtr>(p_ptr);
		if (unlikely(!p_ptr)) {
			return Ref<T>();
		}
		return Ref<T>(reinterpret_cast<T *>(::godot::internal::get_object_instance_binding(::godot::gdextension_interface::ref_get_object(ref))));
	}
};

/**
 * Provides registration metadata for a reference-counted object passed or returned by value.
 */
template <typename T>
struct GetTypeInfo<Ref<T>, typename EnableIf<TypeInherits<RefCounted, T>::value>::type> {
	/**
	 * Registers the reference as an object variant.
	 */
	static constexpr GDExtensionVariantType VARIANT_TYPE = GDEXTENSION_VARIANT_TYPE_OBJECT;
	/**
	 * Indicates that the reference has no additional method-argument metadata.
	 */
	static constexpr GDExtensionClassMethodArgumentMetadata METADATA = GDEXTENSION_METHOD_ARGUMENT_METADATA_NONE;

	/**
	 * Builds object property information with a resource-type hint naming the class.
	 */
	static inline PropertyInfo get_class_info() {
		return make_property_info(Variant::Type::OBJECT, "", PROPERTY_HINT_RESOURCE_TYPE, T::get_class_static());
	}
};

/**
 * Provides registration metadata for a reference-counted object passed by const reference.
 */
template <typename T>
struct GetTypeInfo<const Ref<T> &, typename EnableIf<TypeInherits<RefCounted, T>::value>::type> {
	/**
	 * Registers the const reference as an object variant.
	 */
	static constexpr GDExtensionVariantType VARIANT_TYPE = GDEXTENSION_VARIANT_TYPE_OBJECT;
	/**
	 * Indicates that the const reference has no additional method-argument metadata.
	 */
	static constexpr GDExtensionClassMethodArgumentMetadata METADATA = GDEXTENSION_METHOD_ARGUMENT_METADATA_NONE;

	/**
	 * Builds object property information with a resource-type hint naming the referenced class.
	 */
	static inline PropertyInfo get_class_info() {
		return make_property_info(Variant::Type::OBJECT, "", PROPERTY_HINT_RESOURCE_TYPE, T::get_class_static());
	}
};

} // namespace godot
