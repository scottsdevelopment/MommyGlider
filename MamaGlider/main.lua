-- Main grinding bot script for a Priest
-- This script utilizes the CoreRotations engine for automated gameplay

local CoreRotations = {}

-- Function to perform initial calibration
local function IsCalibration()
    return Calibration() == 0xFFD904
end

-- Helper function to check if a unit's health is below a threshold
local function isHealthBelow(unit, threshold)
    local health = UnitHealth(unit)
    local maxHealth = UnitHealthMax(unit)
    if maxHealth == 0 then return false end
    return (health / maxHealth * 100) < threshold
end

-- Helper function to check if a unit exists
local function UnitExists(unit)
    return UnitHealth(unit) > 0
end

-- Helper function to iterate over units
local function iterateUnits(unitTypes, conditionCallback)
    for _, unit in ipairs(unitTypes) do
        if conditionCallback(unit) then
            return true, unit
        end
    end
    return false, nil
end

-- Helper function for target selection in combat
local function selectTargetInCombat()
    print("Searching for a valid target...")

    if IsSpellInRange("Shoot") and UnitExists("target") and UnitAffectingCombat("target") then
        if UnitThreatSituation("target", "player") >= 1 then
            print("Target found and engaged: target")
            return true
        end
    end

    TargetNearestEnemy() -- Tab to the next enemy

    return false
end

-- Helper function for pre-combat pull logic
local function canPull()
    return IsSpellInRange("Smite") and not UnitAffectingCombat("player") and UnitExists("target") and not UnitAffectingCombat("target") and not UnitDebuff("Shadow Word: Pain", "target")
end

function CoreRotations:addSpellRotation(spell, options)
    self:addRotation({
        condition = function() 
        
            print(spell)

            if GetSpellCooldown(spell) > 0 then return false end
            print(spell)

            if not options.notKnownSpell and 0 == IsSpellKnown(spell) then return false end

            -- Player-specific checks
            local manaPercent = UnitPower("player") / UnitPowerMax("player") * 100
            if options.isMoving and not IsPlayerMoving() then return false end
            if options.minPower and manaPercent < options.minPower then return false end
            if options.maxPower and manaPercent > options.maxPower then return false end
            print(spell)

            if true == options.notInCombat and UnitAffectingCombat("player") then return false end

            local unitsToIterate = {"player", "party1", "party2", "party3", "party4"}
            local unitToTarget = options.unit or "player"

            if unitToTarget == "player" then unitsToIterate = {"player"} end
            if unitToTarget == "target" then unitsToIterate = {"target"} end

            -- Handle party unit iteration
            print(UnitExists("player"))
            local ready, target = iterateUnits(unitsToIterate, function(unit)
                return UnitExists(unit)
                    and (not options.healthThreshold or isHealthBelow(unit, options.healthThreshold))
                    and (not options.maxHealthThreshold or UnitHealth(unit) / UnitHealthMax(unit) * 100 >= options.maxHealthThreshold)
                    and (not options.buff or not UnitBuff(unit, options.buff))
                    and (not options.debuff or not UnitDebuff(unit, options.debuff))
            end)

            print(target)

            if not ready then return false end
            if not UnitExists(target) then return false end
            
            print(target)
            return true, target
        end,
        action = function(target)
            print("Casting " .. spell .. " on " .. (target or "player"))
            if target and target ~= "target" then TargetUnit(target) end
            if not options.notKnownSpell and not IsSpellInRange(spell) then return false end
            CastSpell(spell)
        end
    })
end



-- Initialize the CoreRotations engine
function CoreRotations:initialize()
    self.rotationQueue = {}

    -- Renew while moving
    self:addSpellRotation("Renew", {
        unit = "party",
        isMoving = true,
        buff = "Renew",
        minPower = 25,
        healthThreshold = 95,
    })

    -- Power Word: Shield on party members
    self:addSpellRotation("Power Word: Shield", {
        unit = "party",
        debuff = "Weakened Soul",
        notOnCooldown = true,
        minPower = 45,
    })

    -- Heal party members if health is low
    self:addSpellRotation("Heal", {
        unit = "party",
        healthThreshold = 85,
        minPower = 2,
        cancelSpell = true, -- Cancel current spell if needed
    })

    self:addSpellRotation("Lesser Heal", {
        unit = "party",
        healthThreshold = 85,
        minPower = 30,
        cancelSpell = true, -- Cancel current spell if needed
    })
       
     -- Buff Power Word: Fortitude
    self:addSpellRotation("Power Word: Fortitude", {
        unit = "party",
        buff = "Power Word: Fortitude",
        notInCombat = true,
    })

        -- Drink when mana is between 20% and 65%, stop above 90%
    self:addSpellRotation("Drink", {
        unit = "player",
        maxPower = 60,         -- Stop drinking above 90%
        notInCombat = true,      -- Only drink out of combat
        buff = "Drink",        -- Don't drink if already drinking
        notKnownSpell = true
    })

    -- Eat when health is between 20% and 65%, stop above 90%
    self:addSpellRotation("Eat", {
        unit = "player",
        healthThreshold = 65,  -- Start eating below 65%
        maxHealthThreshold = 90, -- Stop eating above 90%
        notInCombat = true,      -- Only eat out of combat
        buff = "Food",         -- Don't eat if already eating
        notKnownSpell = true
    })  

    -- Target selection for combat
    self:addRotation({
        condition = function()
            return not UnitExists("target") or UnitIsPlayer("target") or not IsSpellInRange("Smite")
        end,
        action = function()
            print("In combat but no valid target.")
            selectTargetInCombat()
        end
    })

 
    -- Mind Blast on target
    self:addSpellRotation("Mind Blast", {
        unit = "target",
        notOnCooldown = true,
    })
    
    -- Add Shadow Word: Pain with debuff maintenance logic
    self:addSpellRotation("Shadow Word: Pain", {
        unit = "target",
        debuff = "Shadow Word: Pain",  -- Maintain the debuff
        maxHealthThreshold = 30,          -- Avoid casting on targets with ≤ 30% health
        minPower = 20,                 -- Avoid casting if mana is below 20%
    })
    
    -- Smite when not on cooldown
    self:addSpellRotation("Smite", {
        unit = "target",
        maxHealthThreshold = 25,          -- Avoid casting on targets with ≤ 30% health
        minPower = 20,
    })

    -- Use wand if low on mana or target is nearly dead
    self:addSpellRotation("Shoot", {
        unit = "target"
    })

    -- Use wand if low on mana or target is nearly dead
    self:addSpellRotation("Attack", {
        unit = "target"
    })
end

-- Add a new rotation to the queue
function CoreRotations:addRotation(rotation)
    table.insert(self.rotationQueue, rotation)
end

-- Execute the rotation queue
function CoreRotations:execute()
    for _, rotation in ipairs(self.rotationQueue) do
        if rotation.condition then
            local result = { rotation.condition() }
            if result[1] then
                rotation.action(table.unpack(result, 2))
                break
            end
        end
    end
end

-- Main game loop
local function gameLoop()
    print("Starting grinding bot...")

    local controlKeyPressed = false

    -- Enter the rotation execution loop
    while true do
        if IsControlKeyDown() and IsCalibration() then
            if not controlKeyPressed then
                CoreRotations:execute()
                controlKeyPressed = true
            end
        else
            controlKeyPressed = false
        end
    end
end

-- Initialize CoreRotations and start the game loop
CoreRotations:initialize()
gameLoop()
