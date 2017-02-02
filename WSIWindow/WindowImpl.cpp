/*
*--------------------------------------------------------------------------
* Copyright (c) 2015-2016 Valve Corporation
* Copyright (c) 2015-2016 LunarG, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Author: Rene Lindsay <rene@lunarg.com>
*
*--------------------------------------------------------------------------
* Platform-specific event handlers call these functions to store input-device state,
* and package the event parameters into a platform-independent "EventType" struct.
*--------------------------------------------------------------------------
*/

#include "WindowImpl.h"

//--Events--
EventType WindowImpl::MouseEvent(eAction action, int16_t x, int16_t y, uint8_t btn) {
    mousepos                           = {x, y};
    if (action != eMOVE) btnstate[btn] = (action == eDOWN);  // Keep track of button state
    EventType e                        = {EventType::MOUSE, {action, x, y, btn}};
    return e;
}

EventType WindowImpl::KeyEvent(eAction action, uint8_t key) {
    keystate[key] = (action == eDOWN);
    EventType e   = {EventType::KEY};
    e.key         = {action, (eKeycode)key};
    return e;
}

EventType WindowImpl::TextEvent(const char* str) {
    EventType e = {EventType::TEXT};
    e.text.str  = str;
    return e;
}

EventType WindowImpl::MoveEvent(int16_t x, int16_t y) {
    shape.x     = x;
    shape.y     = y;
    EventType e = {EventType::MOVE};
    e.move      = {x, y};
    return e;
}

EventType WindowImpl::ResizeEvent(uint16_t width, uint16_t height) {
    shape.width  = width;
    shape.height = height;
    EventType e  = {EventType::RESIZE};
    e.resize     = {width, height};
    return e;
}

EventType WindowImpl::FocusEvent(bool has_focus) {
    this->has_focus   = has_focus;
    EventType e       = {EventType::FOCUS};
    e.focus.has_focus = has_focus;
    return e;
}

EventType WindowImpl::CloseEvent() {
    running = false;
    return {EventType::CLOSE};
}
//----------

void WindowImpl::TextInput(bool enabled) { textinput = enabled; }

bool CSurface::CanPresent(VkPhysicalDevice gpu, uint32_t queue_family) const {
    VkBool32 can_present = false;
#ifdef __LINUX__
    uint gpu_count = 0;
    vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);  // If more than 1 gpu is found,
    if(gpu_count > 1) stderr = freopen(NULL, "r", stderr);  //  mute "Buggy applications may crash" warnings from Mesa
    VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_family, surface, &can_present));
    stderr = freopen(NULL, "a+", stderr);  // Re-enable stderr
#else
    VKERRCHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_family, surface, &can_present));
#endif
    return !!can_present;
}
