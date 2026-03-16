#pragma once

#include <treeItem.hpp>

namespace gurps_system {

// Forward declaration (for documentation, but we include parentRef.hpp below)
class ParentRef;

} // namespace gurps_system

#include "parentRef.hpp"

namespace gurps_system {

/**
 * \brief CRTP mixin providing common ParentRef child management.
 *
 * This class extracts the common pattern of storing a ParentRef as the first
 * child and providing get/set accessors with signal emission. Similar to
 * std::enable_shared_from_this, this uses the Curiously Recurring Template
 * Pattern (CRTP) to allow derived classes to customize signal names.
 *
 * Usage:
 * \code
 * class MyClass : public SingleDependencyHolder<MyClass, BaseClass>
 * {
 * public:
 *   MyClass(boost::uuids::uuid id) : SingleDependencyHolder(id) {}
 *
 *   Q_INVOKABLE infrastructure::TreeItem* myRef() const {
 *     return dependency();
 *   }
 *
 *   Q_INVOKABLE void setMyRef(infrastructure::TreeItem* ref) {
 *     setDependency(ref);
 *   }
 *
 * Q_SIGNALS:
 *   void myRefChanged();
 *
 * protected:
 *   void emitDependencyChanged() override {
 *     Q_EMIT myRefChanged();
 *   }
 * };
 * \endcode
 *
 * \tparam Derived  The derived class (CRTP pattern)
 * \tparam Base     The base class to inherit from
 */
template <typename Derived, typename Base>
class SingleDependencyHolder : public Base
{
protected:
  explicit SingleDependencyHolder(boost::uuids::uuid objectId) : Base(objectId)
  {
    QObject::connect(this, &infrastructure::TreeItem::childInserted, this, [this](int index) {
      if (index == 0 && dynamic_cast<ParentRef*>(this->childAt(0).get())) {
        this->emitDependencyChanged();
      }
    });

    QObject::connect(this, &infrastructure::TreeItem::childAboutToBeRemoved, this,
                     [this](int index) {
                       if (index == 0 && dynamic_cast<ParentRef*>(this->childAt(0).get())) {
                         this->emitDependencyChanged();
                       }
                     });
  }

  /**
   * \brief Returns the ParentRef stored as the first child.
   *
   * Returns \c nullptr if no ParentRef is stored at position 0.
   */
  auto dependency() const -> infrastructure::TreeItem*
  {
    if (this->size() > 0) {
      if (auto ref = std::dynamic_pointer_cast<ParentRef>(this->childAt(0))) {
        return ref.get();
      }
    }
    return nullptr;
  }

  /**
   * \brief Sets the ParentRef as the first child.
   *
   * Replaces any existing child at position 0 if present, otherwise inserts
   * at position 0. Automatically emits the signal via \c emitDependencyChanged().
   *
   * \param ref  The ParentRef to set. Must be a ParentRef* or nullptr.
   */
  auto setDependency(infrastructure::TreeItem* ref) -> void
  {
    if (ref && !dynamic_cast<ParentRef*>(ref)) {
      return; // Not a ParentRef, do nothing
    }

    if (this->size() > 0) {
      // Replace or remove existing child at position 0
      if (ref) {
        this->swapChild(this->childAt(0), ref->shared_from_this());
      } else {
        // Remove the child
        this->removeChild(this->childAt(0));
      }
    } else if (ref) {
      // Insert new child at position 0
      this->insertChild(0, ref->shared_from_this());
    }
  }

  /**
   * \brief Pure virtual function to emit the derived class's specific signal.
   *
   * Derived classes must implement this to emit their custom signal
   * (e.g., targetChanged(), attributeRefChanged(), etc.).
   */
  virtual void emitDependencyChanged() = 0;
};

} // namespace gurps_system
