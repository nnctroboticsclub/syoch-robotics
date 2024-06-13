-- ****************************************************
-- * FEP: FEP
-- ****************************************************

local UVS = Proto.new("robo.uvs", "Robo UVS Protocol")
UVS.fields.length = ProtoField.uint16("robo.uvs.length", "Length")
UVS.fields.opcode = ProtoField.uint8("robo.uvs.opcode", "opcode")
UVS.fields.payload = ProtoField.bytes("robo.uvs.payload", "Payload")

function UVS.dissector(buffer, pinfo, tree)
    local buffer_end = buffer:len()
    local message_count = 0
    local ptr = 0

    while true do
        local packet_top = ptr

        if buffer_end - ptr < 4 then
            ptr = packet_top
            break
        end
        local payload_size = buffer:range(ptr, 4)
        ptr = ptr + 4


        if buffer_end - ptr < payload_size:uint() then
            ptr = packet_top
            break
        end
        local payload = buffer:range(ptr, payload_size:uint())
        ptr = ptr + payload_size:uint()

        opcode = payload:range(0, 1)
        data = payload:range(1)

        local packet_range = buffer:range(packet_top, 4 + payload_size:uint() - 1)
        local subtree = tree:add(UVS, packet_range)
        subtree:add(UVS.fields.length, payload_size)
        subtree:add(UVS.fields.opcode, opcode)
        subtree:add(UVS.fields.payload, data)
        subtree:set_text("Robo (" .. opcode:bytes():tohex() .. ": " .. payload:bytes():tohex() .. ")")


        ptr = ptr + 4 + payload_size:uint()
        message_count = message_count + 1
    end

    pinfo.cols.protocol = "Robo"
    if message_count == 1 then
        pinfo.cols.info = opcode:bytes():tohex() .. ": " .. data:bytes():tohex()
    else
        pinfo.cols.info = "Packets: " .. message_count .. "."
    end
end

local udp_table = DissectorTable.get("udp.port")
udp_table:add(8001, UVS)

-- ****************************************************
-- * FEP: FEP
-- ****************************************************

local FEP = Proto.new("robo.fep", "FEP")
local fep_field_src = ProtoField.uint8("robo.fep.src", "Source")
local fep_field_length = ProtoField.uint8("robo.fep.length", "Length")

FEP.fields = {fep_field_src, fep_field_length}
local FEP_my_heuristic_list = {}
function FEP.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "FEP"

    packet_length = buffer:len()
    local subtree = tree:add(FEP, buffer:range(0, packet_length))

    local offset = 0

    -- Get source string
    subtree:add(fep_field_src,  buffer(offset, 1))
    pinfo.cols.src = buffer(offset, 1):bytes():tohex()
    offset = offset + 1

    -- Get length
    subtree:add(fep_field_length, buffer(offset, 1))
    offset = offset + 1

    local data_buffer = buffer:range(offset,  packet_length-offset)
    local data_tvb = data_buffer:tvb()
    for i, heuristic in ipairs(FEP_my_heuristic_list) do
        if heuristic(data_tvb, pinfo, tree) then
            return
        end
    end

    pinfo.cols.info = data_buffer:bytes():tohex()
end

local function FEP_register_my_heuristic(heuristic)
    table.insert(FEP_my_heuristic_list, heuristic)
end

-- ****************************************************
-- * REP: Reliable FEP Protocol
-- ****************************************************


REP = Proto.new("robo.rep", "Reliable FEP Protocol")
field_key = ProtoField.uint8("robo.rep.key", "Key")
field_length = ProtoField.uint8("robo.rep.length", "Length")
field_check_sum = ProtoField.uint8("robo.rep.checksum", "Checksum")
field_payload = ProtoField.bool("robo.rep.payload", "Payload")
field_key_exchange = ProtoField.bool("robo.rep.key_exchange", "Exchanging")

REP.fields = {field_key, field_length, field_check_sum, field_payload, field_key_exchange}

local REP_my_heuristic_list = {}

function REP.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "REP"

    packet_length = buffer:len()
    local subtree = tree:add(REP, buffer:range(0, packet_length))

    local offset = 3

    length = buffer(offset, 1):uint()
    subtree:add(field_length, length)
    offset = offset + 1

    payload = buffer:bytes(offset, length)
    offset = offset + length

    subtree:add(field_check_sum, buffer(offset, 2))

    pinfo.cols.info = payload:tohex()


    local data_buffer = payload
    local data_tvb = data_buffer:tvb()

    for i, heuristic in ipairs(REP_my_heuristic_list) do
        if heuristic(data_tvb, pinfo, tree) then
            return
        end
    end
end

function rep_check(buffer, pinfo, tree)
    length = buffer:len()

    -- if length < 4 then return false end

    if buffer(0, 1):uint() ~= 0x55 then return false end
    if buffer(1, 1):uint() ~= 0xaa then return false end
    if buffer(2, 1):uint() ~= 0xcc then return false end

    REP.dissector(buffer, pinfo, tree)
    return true
end

local function REP_register_my_heuristic(heuristic)
    table.insert(REP_my_heuristic_list, heuristic)
end

FEP_register_my_heuristic(rep_check)


-- ****************************************************
-- * FEP: Serial Service Protocol
-- ****************************************************

local SSP = Proto.new("robo.ssp", "Serial Service Protocol")
local ssp_field_svc = ProtoField.uint16("robo.ssp.service", "Service Id")
local ssp_field_reserved = ProtoField.uint16("robo.ssp.reserved", "reserved")
local ssp_field_payload = ProtoField.bool("robo.ssp.payload", "Payload")

SSP.fields = {ssp_field_svc, ssp_field_reserved, ssp_field_payload}

local SSP_my_heuristic_list = {}

function SSP.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "SSP"

    packet_length = buffer:len()
    local subtree = tree:add(SSP, buffer:range(0, packet_length))

    local offset = 0

    local service_id = buffer(offset, 2)
    subtree:add(ssp_field_svc,  service_id)
    offset = offset + 2

    local reserved = buffer(offset, 2)
    subtree:add(ssp_field_reserved,  reserved)
    offset = offset + 2

    local payload = buffer(offset, packet_length - offset)

    pinfo.cols.info = service_id:bytes():tohex() .. " " .. payload:bytes():tohex()

    local data_buffer = payload
    local data_tvb = data_buffer:tvb()

    for i, heuristic in ipairs(SSP_my_heuristic_list) do
        if heuristic(data_tvb, pinfo, tree) then
            return
        end
    end
end

function rep_check(buffer, pinfo, tree)
    length = buffer:len()

    if length < 4 then return false end

    if buffer(2, 2):uint() ~= 0x0000 then return false end


    SSP.dissector(buffer, pinfo, tree)
    return true
end

REP_register_my_heuristic(rep_check)