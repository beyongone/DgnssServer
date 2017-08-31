package wits.util;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class StringPairList {

	public static class Pair {
		public String key;
		public String value;
		
		public Pair() {
			
		}
		
		public Pair(String key, String value) {
			this.key = key;
			this.value = value;
		}
	}
	
	protected List< Pair >	pairList;
	
	public StringPairList() {
		pairList = new ArrayList<>();
	}
	
	public int size() {
		return pairList.size();
	}
	
	public boolean isEmpty() {
		return size() == 0;
	}
	
	public void put(String key, String value) {
		Pair pair = find(key);
		if (pair == null) {
			pairList.add(new Pair(key, value));
		} else {
			pair.value = value;
		}
	}
	
	public String get(String key) {
		Pair pair = find(key);
		if (pair == null) {
			return null;
		}
		return pair.value;
	}
		
	public String remove(String key) {
		int idx = findIndex(key);
		if (idx == -1) {
			return null;
		}
		
		return pairList.remove(idx).value;
	}
	
	public void clear() {
		pairList.clear();
	}
	
	public Set<String> keySet() {
		Set<String> keys = new HashSet<String>();
		for (Pair pair: pairList) {
			keys.add(pair.key);
		}
		return keys;
	}
	
	public String[] keys() {
		String[] keys = new String[pairList.size()];
		for (int i = 0; i < pairList.size(); i ++) {
			keys[i] = pairList.get(i).key;
		}
		return keys;
	}
	
	
	public Pair find(String key) {
		for (Pair pair: pairList) {
			if (pair.key.equals(key)) {
				return pair;
			}
		}
		return null;
	}
	
	protected int findIndex(String key) {
		for (int i = 0; i < pairList.size(); i ++) {
			if (pairList.get(i).key.equals(key)) {
				return i;
			}
		}
		return -1;
	}
	
	
	
}
