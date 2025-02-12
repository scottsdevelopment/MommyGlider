-- Function to create offset mapping
local function createOffset(key, fetchFunction, type, hex)
    return { key = key, fetch = fetchFunction, type = type or "number", hex = hex or 0xA0A0A0 }
end

-- Helper function to create C++ DEFINE_MEMORY_OFFSET macros
local function createCppOffset(offset, index)
    local typeMapping = {
        number = "int",
        bool = "bool",
        string = "std::string",
        float = "float"
    }
    local cppType = typeMapping[offset.type] or "int"
    return string.format("DEFINE_MEMORY_OFFSET(%s, %s, %d);", offset.key, cppType, index)
end

-- Helper function to log debug information to chat
local function debugLog(message)
    print("|cFFFFD904[Debug]:|r " .. tostring(message))
end

-- Helper function to set a color as a hex value
local function setHexColor(texture, hex)
    local r = bit.rshift(hex, 16) / 255
    local g = bit.band(bit.rshift(hex, 8), 0xFF) / 255
    local b = bit.band(hex, 0xFF) / 255
    texture:SetColorTexture(r, g, b)
end

-- Function to create offset mapping
local function createOffset(key, fetchFunction, type, hex)
    return { key = key, fetch = fetchFunction, type = type or "number", hex = hex or 0xA0A0A0 }
end

-- Function to encode values into hex color based on type
local function encodeToHex(value, type)
    -- RGB encoding logic (number range)
    if type == "number" then
        return value -- Assume value is already a valid number for RGB encoding
        -- Boolean to RGB encoding (1 = green, 0 = red)
    elseif type == "bool" then
        return value == 1 and 0x00FF00 or 0xFF0000
    else
        return 0x000000 -- Default to black if type is unrecognized
    end
end

-- Define reusable helper function for unit aura checks
local function createAuraOffset(unit, auraType, auraName)
    return createOffset("Unit" .. auraType .. "__" .. unit .. "_" .. auraName:gsub(" ", ""), function()
            for i = 1, 40 do
                local name = (auraType == "Buff") and UnitBuff(unit, i) or UnitDebuff(unit, i)
                if name == auraName then
                    return 1
                end
            end
            return 0
    end, "bool")
end


-- Initialize offsets with dynamic fetch functions

-- Helper function to log debug information to chat
local function debugLog(message)
    print("|cFFFFD904[Debug]:|r " .. tostring(message))
end

-- Helper function to set a color as a hex value
local function setHexColor(texture, hex)
    local r = bit.rshift(hex, 16) / 255
    local g = bit.band(bit.rshift(hex, 8), 0xFF) / 255
    local b = bit.band(hex, 0xFF) / 255
    texture:SetColorTexture(r, g, b)
end

-- Function to create offset mapping
local function createOffset(key, fetchFunction, type, hex)
    return { key = key, fetch = fetchFunction, type = type or "number", hex = hex or 0xA0A0A0 }
end

-- Function to encode values into hex color based on type
local function encodeToHex(value, type, maxValue)
    -- RGB encoding logic (number range)
    if type == "number" then
        return value -- Assume value is already a valid number for RGB encoding
        -- Boolean to RGB encoding (1 = green, 0 = red)
    elseif type == "bool" then
        return value == 1 and 0x00FF00 or 0xFF0000
    else
        return 0x000000 -- Default to black if type is unrecognized
    end
end

-- Define reusable helper function for unit aura checks
local function createAuraOffset(unit, auraType, auraName)
    return createOffset("Unit" .. auraType .. "__" .. unit .. "_" .. auraName:gsub(" ", ""), function()
            for i = 1, 40 do
                local name = (auraType == "Buff") and UnitBuff(unit, i) or UnitDebuff(unit, i)
                if name == auraName then
                    return 1
                end
            end
            return 0
    end, "bool")
end


-- List of unit types and auras to check
local units = { "player", "target", "party1", "party2", "party3", "party4" }
local buffs = { "Power Word: Shield", "Power Word: Fortitude", "Renew" }
local debuffs = { "Shadow Word: Pain", "Weakened Soul" }
local spells = {
    "Power Word: Shield",
    "Mind Blast",
    "Renew",
    "Heal",
    "Smite",
    "Shadow Word: Pain",
    "Power Word: Fortitude",
    "Lesser Heal",
    "Shoot",
    "Attack"
}

-- Initialize offsets with dynamic fetch functions
local function initializeOffsets()
    local offsets = {
        createOffset("Calibration", function() return 0xFFD904 end, "number"),
        createOffset("UnitPosition__player_1", function() return select(1, UnitPosition("player")) end, "number"),
        createOffset("UnitPosition__player_2", function() return select(2, UnitPosition("player")) end, "number"),
        createOffset("UnitPosition__player_3", function() return select(3, UnitPosition("player")) end, "number"),
        createOffset("UnitCastingInfo__player", function() return UnitCastingInfo("player") and 1 or 0 end, "bool"),
        createOffset("UnitChannelInfo__player", function() return UnitChannelInfo("player") and 1 or 0 end, "bool"),
        createOffset("UnitCastingInfo__target", function() return UnitCastingInfo("target") and 1 or 0 end, "bool"),
        createOffset("UnitChannelInfo__target", function() return UnitChannelInfo("target") and 1 or 0 end, "bool"),
        createOffset("UnitCastingInfo__focus", function() return UnitCastingInfo("focus") and 1 or 0 end, "bool"),
        createOffset("UnitChannelInfo__focus", function() return UnitChannelInfo("focus") and 1 or 0 end, "bool"),
        createOffset("UnitPower__player", function() return UnitPower("player") end, "number"),
        createOffset("UnitPowerMax__player", function() return UnitPowerMax("player") end, "number"),
        createOffset("UnitPower__player_0", function() return UnitPower("player", 0) end, "number"),
        createOffset("UnitPower__player_1", function() return UnitPower("player", 1) end, "number"),
        createOffset("IsPlayerMoving", function() return IsPlayerMoving() and 1 or 0 end, "bool"),
        createOffset("IsResting", function() return IsResting() and 1 or 0 end, "bool"),
        createOffset("GetNumQuestLogEntries", function() return GetNumQuestLogEntries() end, "number"),
        createOffset("GetQuestLogCompletionText__3", function() return select(3, GetQuestLogCompletionText()) and 1 or 0 end, "bool"),
        createOffset("UnitXP__player", function() return UnitXP("player") end, "number"),
        createOffset("UnitXPMax__player", function() return UnitXPMax("player") end, "number"),
        createOffset("GetXPExhaustion", function() return GetXPExhaustion() or 0 end, "number"),
        -- createOffset("GetZoneText", function() return GetZoneText() end, "int"),
        -- createOffset("GetSubZoneText", function() return GetSubZoneText() end, "int"),
        createOffset("IsSwimming", function() return IsSwimming() and 1 or 0 end, "bool"),
        createOffset("IsFalling", function() return IsFalling() and 1 or 0 end, "bool"),
        createOffset("GetMoney", function() return GetMoney() end, "number"),
        createOffset("GetNumLootItems", function() return GetNumLootItems() end, "number"),
        createOffset("LootFrame_IsShown", function() return LootFrame:IsShown() and 1 or 0 end, "bool"),
        createOffset("IsControlKeyDown", function() return IsControlKeyDown() and 1 or 0 end, "bool"),
        createOffset("UnitIsPlayer__target", function() return UnitIsPlayer("target") and 1 or 0 end, "bool")
    }
    
    -- Initialize offsets
    for _, unit in ipairs(units) do
        table.insert(offsets, createOffset("UnitHealth__" .. unit, function() return UnitHealth(unit) end, "number"))
        table.insert(offsets, createOffset("UnitHealthMax__" .. unit, function() return UnitHealthMax(unit) end, "number"))
        table.insert(offsets, createOffset("UnitExists__" .. unit, function() return UnitExists(unit) end, "bool"))
        table.insert(offsets, createOffset("UnitAffectingCombat__" .. unit, function() return UnitAffectingCombat(unit) end, "bool"))
        for _, unit2 in ipairs(units) do
            table.insert(offsets, createOffset("UnitThreatSituation__" .. unit .. "_" ..  unit2, function() return UnitThreatSituation(unit, unit2) end, "number"))
        end
        
        
        for _, buff in ipairs(buffs) do
            table.insert(offsets, createAuraOffset(unit, "Buff", buff))
        end
        
        for _, debuff in ipairs(debuffs) do
            table.insert(offsets, createAuraOffset(unit, "Debuff", debuff))
        end
    end
    
    -- Add offsets for spell cooldowns
    for _, spell in ipairs(spells) do
        table.insert(offsets, createOffset("GetSpellCooldown__" .. spell:gsub(" ", ""), function()
                    return GetSpellCooldown(spell)
        end, "number"))
        
        table.insert(offsets,createOffset("IsSpellKnown__"..spell:gsub(" ", ""), function()
                    local spellID = select(7, GetSpellInfo(spell)) 
                    if spellID and IsSpellKnown(spellID)
                    then return 1 else return 0 end
        end, "bool"))
        
        table.insert(offsets, createOffset("IsSpellInRange__" .. spell:gsub(" ", ""), function()
                    return IsSpellInRange(spell) and 1 or 0 
        end, "bool"))
    end
    
    -- Player-specific buffs
    local playerOnlyBuffs = { "Drink", "Food" }
    for _, buff in ipairs(playerOnlyBuffs) do
        table.insert(offsets, createAuraOffset("player", "Buff", buff))
    end
    
    -- Add specific offsets for unique checks
    table.insert(offsets, createOffset("HasWandEquipped", function()
                local wandEquipped = HasWandEquipped()
                return wandEquipped and 1 or 0
    end, "bool"))
    return offsets
end


-- Update offsets dynamically based on their fetch functions
local function updateOffsets(panel, offsets)
    for _, offset in ipairs(offsets) do
        local value = offset.fetch and offset.fetch() or 0
        local hex = encodeToHex(value, offset.type)
        if panel.offsets[offset.key] then
            setHexColor(panel.offsets[offset.key].texture, hex)
        end
    end
end

-- Fetch screen size and scaling
local function fetchScreenData()
    local screenWidth, screenHeight = GetPhysicalScreenSize()
    local uiScale = UIParent:GetScale()
    debugLog("Screen Size: " .. screenWidth .. "x" .. screenHeight .. " Scale: " .. uiScale)
end

local function updateLoop()
    updateOffsets(panel, offsets)
end

-- Generate Memory.h C++ code
local function generateCppFromOffsets()
    local offsets = initializeOffsets()

    for index, offset in ipairs(offsets) do
        print(createCppOffset(offset, index - 1))
    end

end

local function executeForWow()
    -- Initialize and start
    fetchScreenData()
    local offsets = initializeOffsets()
    local size = 2  -- Pixel size
    local spacing = 3 -- Spacing between squares
    local panel = initializePanel(offsets, size, spacing)
    if not aura_env.updateTimer then
        aura_env.updateTimer = C_Timer.NewTicker(0.05, updateLoop)
    end
end
-- Execute the generation
generateCppFromOffsets()

