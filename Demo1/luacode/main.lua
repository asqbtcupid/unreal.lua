function Init()
    package.path = package.path .. ";".._luadir.."\\?.lua"
end

local function write(str)
    local logfile = io.open(_luadir.."\\log.txt", "a")
    logfile:write(str)
    logfile:flush()
    logfile:close()
    print(str)
end
function Q_(t)
    a_(require "inspect"(t))    
end
function a_(...)
    local str = ""
    local temp = {...}
    for i = 1, table.maxn(temp) do
        str = str..tostring(temp[i]).."  "
    end
    str = str.."\n"
    write("["..tostring(os.date()).."] "..str)
end

function test_g()
    Q_(_G)
end
local totaltime = 0
function tick(delta)
    -- collectgarbage("collect")
    -- Q_(debug.getregistry())
    -- totaltime = totaltime + delta
    -- return UGameplayStatics.GetRealTimeSeconds(PawnIns)
    -- return totaltime*factor
    -- totaltime = totaltime + delta
    -- pos = pos or theActor:K2_GetActorLocation()
    -- if theActor then
    --     local newpos = FVector.New()
    --     newpos.X = pos.X
    --     newpos.Y = pos.Y
    --     newpos.Z = pos.Z + 20 * math.sin(totaltime)
    --     theActor:K2_SetActorLocation(newpos, false, FHitResult.New(), false)
    -- end
    if player then
        --player:MoveRight(delta)
    end
end

function testrepush(p)
    player = Afirstperson_1228Character:cast(p)
    -- a_(p.FirstPersonCameraComponent)
    -- a_(p.CapsuleComponent:GetScaledCapsuleRadius())
    -- a_(p.CapsuleComponent:GetShapeScale())
    -- a_("testrepush", p)
end

function testgc(p)
    p = nil
    collectgarbage("collect")
    collectgarbage("collect")
    -- collectgarbage()
    -- collectgarbage()
    -- collectgarbage()
    -- collectgarbage()
    -- local t = {_G, debug.getregistry()}
    -- Q_(t)
    -- collectgarbage("collect")
end

function testreference(p)
    AActor:cast(p)
    local f1 = FVector.New()
    local f2 = FRotator.New()
    local r1, r2 = p:GetActorEyesViewPoint(f1, f2)
    a_(f1.X, f1.Y, f1.Z)
    -- a_(r1.X, r1.Y, r1.Z)
end

function testarray(p)
    AActor:cast(p)
    Q_(p:GetAllChildActors())
end

function testenum( ... )
    -- Q_(_G)
    a_(collectgarbage("count"))
end

function setpawn(p)
    AActor:cast(p)
    local t = {}
    t._cppinstance_ = p
    a_(AActor.K2_GetRootComponent(t))
    t = nil
    p = nil
    collectgarbage("collect")
    -- a_(p:K2_GetRootComponent())
    -- a_(p.FolderPath)
end

function teststr(hehe)
    return hehe
end

function testpath()
    a_(_luadir)
    a_(require "test")
end

function testactor(p)
    theActor = AActor:cast(p)
    -- Q_(UActorComponent)
    Q_(theActor:GetComponentsByClass(UActorComponent:Class()))
    -- a_(UStaticMesh:Class())
end