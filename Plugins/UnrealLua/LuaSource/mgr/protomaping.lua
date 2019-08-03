local protomap = {}
require "proto.cmd_pb"
local name2file = require "proto.msgnametofile_pb"

protomap["CSActvDescRes"] = {30002}
protomap["CSActvListRes"] = {30004}
protomap["CSActvExchangeRes"] = {30011}
protomap["CSActvAwardRes"] = {30007}
protomap["CSActvAwardListRes"] = {30020}
protomap["CSActvDoExchangeRes"] = {30013}
protomap["CSActvUpdateNtf"] = {30005}
protomap["CSActvClientNoticeRes"] = {30009}
protomap["CSActvGetInCrementDescRes"] = {30022}

protomap["CSActvDescReq"] = {30001}
protomap["CSActvListReq"] = {30003}
protomap["CSActvExchangeReq"] = {30010}
protomap["CSActvAwardReq"] = {30006}
protomap["CSActvAwardListReq"] = {30019}
protomap["CSActvDoExchangeReq"] = {30012}
protomap["CSActvGetInCrementDescReq"] = {30021}

local id2namemaping = {}
for k, v in pairs(protomap) do
	v[2] = name2file[k].."_pb"
	v[3] = k
	id2namemaping[v[1]] = v
end

for k, v in pairs(id2namemaping) do
	protomap[k] = v
end

return protomap
