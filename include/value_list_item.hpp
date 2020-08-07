#pragma once

#include <list>
#include <cstdint>
#include <tesla.hpp>
#include "gui_sublist.hpp"

class ValueListItem : public tsl::elm::ListItem
{
public:
  ValueListItem(
    std::string text,
    const std::vector<std::string> values,
    int* defaultPos,
    const std::string data,
    const std::string help) :
  tsl::elm::ListItem(text), m_values(values), m_curValue(defaultPos), extdata(data), help(help)
  {
    this->setValue(m_values[*m_curValue]);

    this->initEventListener();
  }

  ~ValueListItem()
  {
  }

  void applyChanges(){
        int value = this->getCurValue();
        this->setValue(this->getValues()[value]);
        this->setCurValue(value);
  }

  tsl::elm::Element *requestFocus(Element *oldFocus, tsl::FocusDirection direction)
  {
    return ListItem::requestFocus(oldFocus, direction);
  }

  void layout(u16 parentX, u16 parentY, u16 parentWidth, u16 parentHeight)
  {
    ListItem::layout(parentX, parentY, parentWidth, parentHeight);
  }

  bool onClick(u64 keys)
  {
    if (keys & KEY_A)
    {

      if (this->m_valueChangeListener != nullptr) {
        this->m_valueChangeListener(this->m_values, this->m_curValue, this->extdata, this->getText(), this->help);
        return true;
      }

      return true;
    }

    return false;
  }

  int getCurValue()
  {
    return *(this->m_curValue);
  }
  void setCurValue(int value) { *(this->m_curValue) = value; }

  const std::string getExtData() { return this->extdata; }

  const std::vector<std::string> getValues() { return this->m_values; }

  void setStateChangedListener(std::function<void(const std::vector<std::string>, int*, std::string, std::string, std::string)> valueChangeListener) { this->m_valueChangeListener = valueChangeListener; }

protected:
  void initEventListener()
  {
    this->setStateChangedListener([](std::vector<std::string> menuItems, int *val, std::string data, std::string title, std::string helpTxt) {
        tsl::changeTo<GuiSublist>(menuItems, val, data, title, helpTxt);
    });
  }


private:
  const std::vector<std::string> m_values;
  int* m_curValue;
  std::function<void(const std::vector<std::string>, int*, std::string, std::string, std::string)> m_valueChangeListener = nullptr;
  const std::string extdata;
  const std::string help;
};

class ToggleBitListItem : public tsl::elm::ToggleListItem
{
  uint32_t m_mask;
  uint32_t* m_value;
public:
  ToggleBitListItem(const std::string& text, uint32_t mask, uint32_t* value)
    : tsl::elm::ToggleListItem(text, (mask & *value) != 0), m_mask(mask), m_value(value)
  {
    setStateChangedListener([this](bool v)
    {
      if(v)
      {
        *m_value |= m_mask;
      }
      else
      {
        *m_value &= ~m_mask;
      }
    });
  }
  virtual ~ToggleBitListItem() = default;
};

class SetToggleBitListItem : public tsl::elm::ListItem
{
  std::vector<ToggleBitListItem*> m_itemsOn;
  std::vector<ToggleBitListItem*> m_itemsOff;
public:
  SetToggleBitListItem(std::vector<ToggleBitListItem*> itemsOn, std::vector<ToggleBitListItem*> itemsOff, const std::string& text, const std::string& value = "")
    : tsl::elm::ListItem(text, value), m_itemsOn(std::move(itemsOn)), m_itemsOff(std::move(itemsOff))
    {
      setClickListener([this](u64 keys)->bool
      {
        if(keys & KEY_A)
        {
          for(auto it : m_itemsOn)
          {
            it->setState(true); 
          }
          for(auto it : m_itemsOff)
          {
            it->setState(false);
          }
          return true;
        }
        return false;
      });
    }
  virtual ~SetToggleBitListItem() = default;
};