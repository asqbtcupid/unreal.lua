function MathDis(a, b)
	local total = 0
	for k, v in pairs(a) do
		total = total + (v-b[k])^2
	end
	return math.sqrt(total)
end
math.Dis = MathDis